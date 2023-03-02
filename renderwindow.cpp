#include "renderwindow.h"
#include <QTimer>
#include <QMatrix4x4>
#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QOpenGLDebugLogger>
#include <QKeyEvent>
#include <QStatusBar>
#include <QDebug>
#include <QVector3D>


#include "qthread.h"

#include <string>

#include "shader.h"
#include "mainwindow.h"
#include "logger.h"
#include "xyz.h"
#include "tetrahedron.h"
#include "cube.h"
#include "curves.h"
#include "collisionvolume.h"
#include "interactivecollisionvolume.h"
#include "plane.h"

RenderWindow::RenderWindow(const QSurfaceFormat &format, MainWindow *mainWindow)
    : mContext(nullptr), mInitialized(false), mMainWindow(mainWindow)

{
    //This is sent to QWindow:
    setSurfaceType(QWindow::OpenGLSurface);
    setFormat(format);
    //Make the OpenGL context
    mContext = new QOpenGLContext(this);
    //Give the context the wanted OpenGL format (v4.1 Core)
    mContext->setFormat(requestedFormat());
    if (!mContext->create()) {
        delete mContext;
        mContext = nullptr;
        qDebug() << "Context could not be made - quitting this application";
    }

    //This is the matrix used to transform (rotate) the triangle
    //You could do without, but then you have to simplify the shader and shader setup
    mMVPmatrix = new QMatrix4x4{};
    mMVPmatrix->setToIdentity();

    mPmatrix = new QMatrix4x4{};
    mPmatrix->setToIdentity();    //1, 1, 1, 1 in the diagonal of the matrix
    mVmatrix = new QMatrix4x4{};
    mVmatrix->setToIdentity();    //1, 1, 1, 1 in the diagonal of the matrix

    //Make the gameloop timer:
    mRenderTimer = new QTimer(this);

    //mia = new InteractiveObject;
    //mObjects.push_back(mia);
    //mObjects.push_back(new TriangleSurface());
    //mObjects.push_back(new XYZ());
    //mObjects.push_back(new Tetrahedron());
    //mObjects.push_back(new Cube());
    //mObjects.push_back(new OctahedronBall(5));
    //mObjects.push_back(new Disc());

    //    mMap.insert(std::pair<std::string, VisualObject*>{"triangleSurface",new TriangleSurface("frankes.txt")});
    //    std::pair<std::string, VisualObject*> par{"disc", new Disc{}};
    //    mMap.insert(par);

    mMap.insert(std::pair<std::string, VisualObject*>{"xyz",new XYZ{}});
    mMap.insert(std::pair<std::string, VisualObject*>{"curve", new Curves("curve.txt")});

    mia = new InteractiveObject;
    miaCollision = new InteractiveCollisionVolume(1);
    mMap.insert(std::pair<std::string, VisualObject*> {"mia", mia});
    mMap.insert(std::pair<std::string, VisualObject*> {"miaCollision", miaCollision});

    BOT = new NPC;
    mMap.insert(std::pair<std::string, VisualObject*> {"NPC", BOT});

    mMap.insert(std::pair<std::string, VisualObject*>{"house",new Cube(2,   -6, -1.5, 0,    1, 1, 1)});
    //mMap.insert(std::pair<std::string, VisualObject*>{"door",new Plane{}});

    //mMap.at("house")->SetRotation();

    //Oppgave 1 OBLIG 2
    mObjects.push_back(new OctahedronBall(-3,-1, 3));
    mObjects.push_back(new OctahedronBall(-2,-1, 3));
    mObjects.push_back(new OctahedronBall(-1.5,-2, 3));
    mObjects.push_back(new OctahedronBall(-1,1, 3));
    mObjects.push_back(new OctahedronBall(1,1, 3));
    mObjects.push_back(new OctahedronBall(1,-1, 3));
    mObjects.push_back(new OctahedronBall(2,2, 3));
    mObjects.push_back(new OctahedronBall(3,2, 3));
    mObjects.push_back(new Plane());
    //amObjects.push_back(new OctahedronBall(-4.5, -0.7, 1));

    // Kollisjonsvolum               //(radius, x, y, recursions)
    mItems.push_back(new CollisionVolume(0.2, -3, -1, 1));
    mItems.push_back(new CollisionVolume(0.2, -2, -1, 1));
    mItems.push_back(new CollisionVolume(0.2, -1.5, -2, 1));
    mItems.push_back(new CollisionVolume(0.2, -1, 1, 1));
    mItems.push_back(new CollisionVolume(0.2, 1, 1, 1));
    mItems.push_back(new CollisionVolume(0.2, 1, -1, 1));
    mItems.push_back(new CollisionVolume(0.2, 2, 2, 1));
    mItems.push_back(new CollisionVolume(0.2, 3, 2, 1));
    // Door
    mItems.push_back(new CollisionVolume(1, -4.5, -0.7, 1));



    // Oppgave 2 OBLIG 2
    //mMap.insert(std::pair<std::string, VisualObject*>{"P5", new Tetrahedron(-3,-3)});
    //mMap.insert(std::pair<std::string, VisualObject*>{"P6", new Tetrahedron(-2, 2)});
    //mMap.insert(std::pair<std::string, VisualObject*>{"P7", new Tetrahedron( 2,-2)});
    //mMap.insert(std::pair<std::string, VisualObject*>{"P8", new Tetrahedron( 3, 3)});
}

RenderWindow::~RenderWindow()
{
    //cleans up the GPU memory
    glDeleteVertexArrays( 1, &mVAO );
    glDeleteBuffers( 1, &mVBO );
}

//Simple global for vertices of a triangle - should belong to a class !
static GLfloat vertices[] =
{
    // Positions         // Colors
    -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,  // Bottom Left
    0.5f, -0.5f, 0.0f,   1.0f, 0.0f, 0.0f,  // Bottom Right
    0.0f,  0.5f, 0.0f,   0.0f, 0.0f, 1.0f   // Top
};

// Sets up the general OpenGL stuff and the buffers needed to render a triangle
void RenderWindow::init()
{
    //Get the instance of the utility Output logger
    //Have to do this, else program will crash (or you have to put in nullptr tests...)
    mLogger = Logger::getInstance();

    //Connect the gameloop timer to the render function:
    //This makes our render loop
    connect(mRenderTimer, SIGNAL(timeout()), this, SLOT(render()));
    //********************** General OpenGL stuff **********************

    //The OpenGL context has to be set.
    //The context belongs to the instanse of this class!
    if (!mContext->makeCurrent(this)) {
        mLogger->logText("makeCurrent() failed", LogType::REALERROR);
        return;
    }

    //just to make sure we don't init several times
    //used in exposeEvent()
    if (!mInitialized)
        mInitialized = true;

    //must call this to use OpenGL functions
    initializeOpenGLFunctions();

    //Print render version info (what GPU is used):
    //Nice to see if you use the Intel GPU or the dedicated GPU on your laptop
    // - can be deleted
    mLogger->logText("The active GPU and API:", LogType::HIGHLIGHT);
    std::string tempString;
    tempString += std::string("  Vendor: ") + std::string((char*)glGetString(GL_VENDOR)) + "\n" +
            std::string("  Renderer: ") + std::string((char*)glGetString(GL_RENDERER)) + "\n" +
            std::string("  Version: ") + std::string((char*)glGetString(GL_VERSION));
    mLogger->logText(tempString);

    //Start the Qt OpenGL debugger
    //Really helpfull when doing OpenGL
    //Supported on most Windows machines - at least with NVidia GPUs
    //reverts to plain glGetError() on Mac and other unsupported PCs
    // - can be deleted
    startOpenGLDebugger();

    //general OpenGL stuff:
    glEnable(GL_DEPTH_TEST);            //enables depth sorting - must then use GL_DEPTH_BUFFER_BIT in glClear
    //    glEnable(GL_CULL_FACE);       //draws only front side of models - usually what you want - test it out!
    glClearColor(0.2f, 0.0f, 0.4f, 1.0f);    //gray color used in glClear GL_COLOR_BUFFER_BIT

    //Compile shaders:
    //NB: hardcoded path to files! You have to change this if you change directories for the project.
    //Qt makes a build-folder besides the project folder. That is why we go down one directory
    // (out of the build-folder) and then up into the project folder.
    mShaderProgram = new Shader("C:/4 semester/3D Prog/3Dprog22/3Dprog22/plainshader.vert",
                                "C:/4 semester/3D Prog/3Dprog22/3Dprog22/plainshader.frag");

    mPmatrixUniform = glGetUniformLocation( mShaderProgram->getProgram(), "pmatrix" );
    mVmatrixUniform = glGetUniformLocation( mShaderProgram->getProgram(), "vmatrix" );
    mMmatrixUniform = glGetUniformLocation( mShaderProgram->getProgram(), "matrix" );

   // mCamera.init(mPmatrixUniform, mVmatrixUniform);

    // Pickups
    for (auto it=mObjects.begin(); it!=mObjects.end(); it++)
        (*it)->init(mMmatrixUniform);

    // Kollisjonsvolum
    for (auto it=mItems.begin(); it!=mItems.end(); it++)
        (*it)->init(mMmatrixUniform);

    // Alternativ: Erstatter std::vector<VisualObject*> med unordered map
    for (auto it=mMap.begin(); it!=mMap.end(); it++)
        (*it).second->init(mMmatrixUniform);

    glBindVertexArray(0);       //unbinds any VertexArray - good practice
    miaCollision->move(0,0,0);
}

// Called each frame - doing the rendering!!!
void RenderWindow::render()
{
    mCamera.init(mPmatrixUniform, mVmatrixUniform);
    mCamera.perspective(70.0f, 16.0f/9.0f, 0.1f, 10.f);

    mTimeStart.restart(); //restart FPS clock
    mContext->makeCurrent(this); //must be called every frame (every time mContext->swapBuffers is called)

    initializeOpenGLFunctions();    //must call this every frame it seems...

    //clear the screen for each redraw
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //what shader to use
    glUseProgram(mShaderProgram->getProgram());


    if (bSceneOne)
        // Scene 1
        mCamera.lookAt( QVector3D{0,0,5}, QVector3D{0,0,0}, QVector3D{0,1,0} );
    else
        // Scene 2
        mCamera.lookAt( QVector3D{0,0,5}, QVector3D{-50,-50,0}, QVector3D{0,1,0} );


    mCamera.update();
    //for (auto it = mObjects.begin(); it != mObjects.end(); it++)
    //    (*it)->draw();

    // Alternativ: Erstatter std::vector<VisualObject*> med unordered map
    for (auto it=mMap.begin(); it!=mMap.end(); it++)
        (*it).second->draw();

    for (auto it=mItems.begin(); it!=mItems.end(); it++)
        (*it)->draw();

    for (auto it = mObjects.begin(); it != mObjects.end(); it++)
        (*it)->draw();


    if (mia && miaCollision)
    {
        mia->getPosition();
        miaCollision->getPosition();

        if (miaCollision->getPosition().x() < - 5)
        {
            bSceneOne = false;

            mia->move(-0.1,-0.1,0);
            miaCollision->move(-0.1,-0.1,0);

            if (miaCollision->getPosition().x() <= -10 || miaCollision->getPosition().y() <= -10)
            {
                mia->move(0.1,0.1,0);
                miaCollision->move(0.1,0.1,0);
            }

        }
    }


    //mMap["disc"]->move(0.05f);

    //Calculate framerate before
    // checkForGLerrors() because that call takes a long time
    // and before swapBuffers(), else it will show the vsync time
    calculateFramerate();

    //using our expanded OpenGL debugger to check if everything is OK.
    checkForGLerrors();

    //Qt require us to call this swapBuffers() -function.
    // swapInterval is 1 by default which means that swapBuffers() will (hopefully) block
    // and wait for vsync.
    mContext->swapBuffers(this);

    //just to make the triangle rotate - tweak this:
    //                   degree, x,   y,   z -axis
    if(mRotate)
        mPmatrix->rotate(2.f, 0.f, 1.0, 0.f);

    // "Kollisjon"
    for (auto i = 0; i < mItems.size(); i++)
    {
        // Finne avstand mellom posisjoner
        QVector3D dist = miaCollision->getPosition() - mItems[i]->getPosition();
        // [x1 – x2, y1 – y2, z1 – z2]
        float d = dist.length();
        //sqrt((x1 – x2)^2, (y1 – y2)^2, (z1 – z2)^2)
        float r1 = miaCollision->getRadius(); // Må legges til
        float r2 = mItems[i]->getRadius();

        if (d < r1 + r2 && mItems[i]->bIsActive == true)
        {
            // slå av rendering / flytt ob
            mItems[i]->move(100,100,-100);
            mItems[i]->bIsActive = false;
            if (mObjects[i])
            {
                mObjects[i]->move(100,100,-100);
                mObjects[i]->OpenDoor();
            }
        }
    }
}

//This function is called from Qt when window is exposed (shown)
// and when it is resized
//exposeEvent is a overridden function from QWindow that we inherit from
void RenderWindow::exposeEvent(QExposeEvent *)
{
    //if not already initialized - run init() function - happens on program start up
    if (!mInitialized)
        init();

    //This is just to support modern screens with "double" pixels (Macs and some 4k Windows laptops)
    const qreal retinaScale = devicePixelRatio();

    //Set viewport width and height to the size of the QWindow we have set up for OpenGL
    glViewport(0, 0, static_cast<GLint>(width() * retinaScale), static_cast<GLint>(height() * retinaScale));

    //If the window actually is exposed to the screen we start the main loop
    //isExposed() is a function in QWindow
    if (isExposed())
    {
        //This timer runs the actual MainLoop
        //16 means 16ms = 60 Frames pr second (should be 16.6666666 to be exact...)
        mRenderTimer->start(16);
        mTimeStart.start();
    }
}

//The way this function is set up is that we start the clock before doing the draw call,
// and check the time right after it is finished (done in the render function)
//This will approximate what framerate we COULD have.
//The actual frame rate on your monitor is limited by the vsync and is probably 60Hz
void RenderWindow::calculateFramerate()
{
    long nsecElapsed = mTimeStart.nsecsElapsed();
    static int frameCount{0};                       //counting actual frames for a quick "timer" for the statusbar

    if (mMainWindow)            //if no mainWindow, something is really wrong...
    {
        ++frameCount;
        if (frameCount > 30)    //once pr 30 frames = update the message == twice pr second (on a 60Hz monitor)
        {
            //showing some statistics in status bar
            mMainWindow->statusBar()->showMessage(" Time pr FrameDraw: " +
                                                  QString::number(nsecElapsed/1000000.f, 'g', 4) + " ms  |  " +
                                                  "FPS (approximated): " + QString::number(1E9 / nsecElapsed, 'g', 7));
            frameCount = 0;     //reset to show a new message in 30 frames
        }
    }
}

//Uses QOpenGLDebugLogger if this is present
//Reverts to glGetError() if not
void RenderWindow::checkForGLerrors()
{
    if(mOpenGLDebugLogger)  //if our machine got this class to work
    {
        const QList<QOpenGLDebugMessage> messages = mOpenGLDebugLogger->loggedMessages();
        for (const QOpenGLDebugMessage &message : messages)
        {
            if (!(message.type() == message.OtherType)) // get rid of uninteresting "object ...
                                                        // will use VIDEO memory as the source for
                                                        // buffer object operations"
                // valid error message:
                mLogger->logText(message.message().toStdString(), LogType::REALERROR);
        }
    }
    else
    {
        GLenum err = GL_NO_ERROR;
        while((err = glGetError()) != GL_NO_ERROR)
        {
            mLogger->logText("glGetError returns " + std::to_string(err), LogType::REALERROR);
            switch (err) {
            case 1280:
                mLogger->logText("GL_INVALID_ENUM - Given when an enumeration parameter is not a "
                                "legal enumeration for that function.");
                break;
            case 1281:
                mLogger->logText("GL_INVALID_VALUE - Given when a value parameter is not a legal "
                                "value for that function.");
                break;
            case 1282:
                mLogger->logText("GL_INVALID_OPERATION - Given when the set of state for a command "
                                "is not legal for the parameters given to that command. "
                                "It is also given for commands where combinations of parameters "
                                "define what the legal parameters are.");
                break;
            }
        }
    }
}

//Tries to start the extended OpenGL debugger that comes with Qt
//Usually works on Windows machines, but not on Mac...
void RenderWindow::startOpenGLDebugger()
{
    QOpenGLContext * temp = this->context();
    if (temp)
    {
        QSurfaceFormat format = temp->format();
        if (! format.testOption(QSurfaceFormat::DebugContext))
            mLogger->logText("This system can not use QOpenGLDebugLogger, so we revert to glGetError()",
                             LogType::HIGHLIGHT);

        if(temp->hasExtension(QByteArrayLiteral("GL_KHR_debug")))
        {
            mLogger->logText("This system can log extended OpenGL errors", LogType::HIGHLIGHT);
            mOpenGLDebugLogger = new QOpenGLDebugLogger(this);
            if (mOpenGLDebugLogger->initialize()) // initializes in the current context
                mLogger->logText("Started Qt OpenGL debug logger");
        }
    }
}

//Event sent from Qt when program receives a keyPress
// NB - see renderwindow.h for signatures on keyRelease and mouse input
void RenderWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape)
    {
        mMainWindow->close();       //Shuts down the whole program
    }
    //You get the keyboard input like this
    if(event->key() == Qt::Key_A)
    {
        moveMiaX(-0.2f);
    }
    if(event->key() == Qt::Key_D)
    {
        moveMiaX(0.2f);
    }
    if(event->key() == Qt::Key_W)
    {
        moveMiaY(0.2f);
    }
    if(event->key() == Qt::Key_S)
    {
        moveMiaY(-0.2f);
    }

    //std::cout << "WorldPos: \n";
    //std::cout << "x: " << miaCollision->getPosition().x() << " y: " << miaCollision->getPosition().y() << " z: " << miaCollision->getPosition().z() << "\n";
}

void RenderWindow::moveMiaX(float movespeed)
{
    miaCollision->move(movespeed, 0.0f, 0.0f);
    miaCollision->mWorldPosition += QVector3D{movespeed * miaCollision->getRadius(), 0.0f, 0.0f};
    mMap["mia"]->move(movespeed * miaCollision->getRadius(), 0.0f, 0.0f);
}

void RenderWindow::moveMiaY(float movespeed)
{
    miaCollision->move(0.0f, movespeed, 0.0f);
    miaCollision->mWorldPosition += QVector3D{0.0f, movespeed * miaCollision->getRadius(), 0.0f};
    mMap["mia"]->move(0.0f, movespeed * miaCollision->getRadius(), 0.0f);
}
