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

#include <glm.hpp>

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
#include "triangulation.h"

typedef std::pair<std::string, VisualObject*> MapPair;

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

    //mMap.insert(MapPair{"Surface",new TriangleSurface("frankes.txt")});
    mMap.insert(MapPair{"xyz",new XYZ{}});

    Path = new Curves("curve.txt");
    Path2 = new Curves("curve2.txt");
    mMap.insert(MapPair{"Path", Path});
    mMap.insert(MapPair{"Path2", Path2});

    mMap.insert(MapPair{"house",new Cube(2,   -6, -1.5, 0,    0.8, 0.6, 0.3)});
    mMap.insert(MapPair{"house2",new Cube(4,   -12, -12, -0.1,    0.8, 0.6, 0.3)});
    mMap.insert(MapPair{"Floor", new Plane(-12.5,-7.5,5)});
    mMap.insert(MapPair{"Object", new Tetrahedron(-10,-10, 0.2, 1.2)});

    //mMap.insert(MapPair{"Triangulation", (new Triangulation())});

    //Oppgave 1 OBLIG 2
    mObjects.push_back(new OctahedronBall(-3,-1, 3));
    mObjects.push_back(new OctahedronBall(-2,-1, 3));
    mObjects.push_back(new OctahedronBall(-1.5,-2, 3));
    mObjects.push_back(new OctahedronBall(-1,1, 3));
    mObjects.push_back(new OctahedronBall(1,1, 3));
    mObjects.push_back(new OctahedronBall(1,-1, 3));
    mObjects.push_back(new OctahedronBall(2,2, 3));
    mObjects.push_back(new OctahedronBall(3,2, 3));
    //Door
    mObjects.push_back(new Plane());

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
    mItems.push_back(new CollisionVolume(1.25, -4.5, -0.5, 2));

    // Oppgave 2 OBLIG 2
    mMap.insert(std::pair<std::string, VisualObject*>{"P1", new Tetrahedron(-3,-3, 0, 0.5)});
    mMap.insert(std::pair<std::string, VisualObject*>{"P2", new Tetrahedron(-2, 2, 0, 0.5)});
    mMap.insert(std::pair<std::string, VisualObject*>{"P3", new Tetrahedron( 2,-2, 0, 0.5)});
    mMap.insert(std::pair<std::string, VisualObject*>{"P4", new Tetrahedron( 3, 3, 0, 0.5)});

    BOT = new NPC("curve.txt");
    BOT2 = new NPC("curve2.txt");
    mMap.insert(MapPair{"NPC", BOT});
    mMap.insert(MapPair{"NPC2", BOT2});
    triangulation = new Triangulation();

    mia = new InteractiveObject;
    miaCollision = new InteractiveCollisionVolume(1);
    //mMap.insert(MapPair{"mia", mia});
    mMap.insert(MapPair{"miaCollision", miaCollision});
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
    mPlainShaderProgram = new Shader( "C:/4 semester/3D Prog/3Dprog22/3Dprog22/plainshader.vert",
                                    "C:/4 semester/3D Prog/3Dprog22/3Dprog22/plainshader.frag");

    mTexShaderProgram = new Shader( "C:/4 semester/3D Prog/3Dprog22/3Dprog22/texshader.vert",
                                    "C:/4 semester/3D Prog/3Dprog22/3Dprog22/texshader.frag");

    mPhongShaderProgram = new Shader( "C:/4 semester/3D Prog/3Dprog22/3Dprog22/phongshader.vert",
                                    "C:/4 semester/3D Prog/3Dprog22/3Dprog22/phongshader.frag");

    setupPlainShader();
    setupTextureShader();
    setupPhongShader();

    brickTexture = new Texture((char*)("Textures/brick.png"));
    brickTexture->LoadTexture();

   // mCamera.init(mPmatrixUniform, mVmatrixUniform);

    // Pickups
    for (auto it=mObjects.begin(); it!=mObjects.end(); it++)
        (*it)->init(mMmatrixUniform0);

    // Kollisjonsvolum
    for (auto it=mItems.begin(); it!=mItems.end(); it++)
        (*it)->init(mMmatrixUniform0);

    // Alternativ: Erstatter std::vector<VisualObject*> med unordered map
    for (auto it=mMap.begin(); it!=mMap.end(); it++)
        (*it).second->init(mMmatrixUniform0);

    triangulation->init(mMmatrixUniform0);

    mia->init(mMmatrixUniform1);

    glBindVertexArray(0);       //unbinds any VertexArray - good practice

    // Additional setup
    //moveMiaX(-1);
    //moveMiaY(-3);

    BOT2->bShouldRender = false;
    Path2->bShouldRender = false;

    auto v = triangulation->GetTriangles();
    for (int i = 0; i < v.size(); i++)
    {
        glm::vec2 xy;
        xy.x = v[i].getX();
        xy.y = v[i].getY();

        mTriangles.push_back(xy);
    }
}

// Called each frame - doing the rendering!!!
void RenderWindow::render()
{
    mCamera.init(mPmatrixUniform0, mVmatrixUniform0);
    mCamera.perspective(60.0f, 16.0f/9.0f, 0.1f, 20.f);

    mTimeStart.restart(); //restart FPS clock
    mContext->makeCurrent(this); //must be called every frame (every time mContext->swapBuffers is called)

    initializeOpenGLFunctions();    //must call this every frame it seems...

    //clear the screen for each redraw
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //what shader to use (plain shader)
    glUseProgram(mPlainShaderProgram->getProgram());
    glUniformMatrix4fv(mVmatrixUniform0, 1, GL_TRUE, mCamera.mVmatrix.constData());
    glUniformMatrix4fv(mPmatrixUniform0, 1, GL_TRUE, mCamera.mPmatrix.constData());

    if (bSceneOne)
        // Scene 1
        //mCamera.lookAt( QVector3D{-0,-4,4}, QVector3D{0,-1,0}, QVector3D{0,1,0} );
        mCamera.lookAt( QVector3D{1, 0,10}, QVector3D{1,1,0}, QVector3D{0,1,0} );
    else
        // Scene 2
        mCamera.lookAt( QVector3D{-10,-10,3}, QVector3D{-10,-10,0}, QVector3D{0,1,0} );

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

    triangulation->draw();

    // what shader to use (texture shader)
    glUseProgram(mTexShaderProgram->getProgram());
    glUniformMatrix4fv(mVmatrixUniform1, 1, GL_TRUE, mCamera.mVmatrix.constData());
    glUniformMatrix4fv(mPmatrixUniform1, 1, GL_TRUE, mCamera.mPmatrix.constData());
    // tex-shader
    glUniform1i(mTextureUniform1, 0);
    brickTexture->UseTexture();
    // need to update camera to apply changes!
    mCamera.update();

    mia->draw();

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
    //if(mRotate)
        //mPmatrix->rotate(2.f, 0.f, 1.0, 0.f);

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
            mItems[i]->move(100,100,100);
            mItems[i]->bIsActive = false;
            if (mObjects[i])
            {
                mObjects[i]->bShouldRender = false;
                mObjects[i]->OpenDoor();
            }
        }
    }
    if (mia && miaCollision)
    {
        mia->getPosition();
        miaCollision->getPosition();

        if (miaCollision->getPosition().x() < -4.5
            && miaCollision->getPosition().y() <= 1
            && miaCollision->getPosition().y() >= -1)
        {
            bSceneOne = false;
            if (bShouldMove)
            {
                moveMiaX(-4);
                moveMiaY(-9.5);

                bShouldMove = false;
            }
        }
    }
    ToggleCollision();
    TogglePath();

    for (int i = 0; i < mTriangles.size() - 2; i++)
    {
        mia->barycentricCoordinates(mTriangles[i], mTriangles[i + 1], mTriangles[i + 2]);
    }
}

void RenderWindow::setupPlainShader()
{
    mPmatrixUniform0 =  glGetUniformLocation( mPlainShaderProgram->getProgram(), "pmatrix" );
    mVmatrixUniform0 =  glGetUniformLocation( mPlainShaderProgram->getProgram(), "vmatrix" );
    mMmatrixUniform0 =  glGetUniformLocation( mPlainShaderProgram->getProgram(), "matrix" );
}

void RenderWindow::setupTextureShader()
{
    mPmatrixUniform1 =  glGetUniformLocation( mTexShaderProgram->getProgram(), "pmatrix" );
    mVmatrixUniform1 =  glGetUniformLocation( mTexShaderProgram->getProgram(), "vmatrix" );
    mMmatrixUniform1 =  glGetUniformLocation( mTexShaderProgram->getProgram(), "matrix" );
    // Add sampler uniform to the shader
    mTextureUniform1  =  glGetUniformLocation( mTexShaderProgram->getProgram(), "textureSampler");
}

void RenderWindow::setupPhongShader()
{
    mPmatrixUniform2 =  glGetUniformLocation( mPhongShaderProgram->getProgram(), "pmatrix" );
    mVmatrixUniform2 =  glGetUniformLocation( mPhongShaderProgram->getProgram(), "vmatrix" );
    mMmatrixUniform2 =  glGetUniformLocation( mPhongShaderProgram->getProgram(), "matrix" );
    // Add sampler uniform to the shader
    mTextureUniform2  =  glGetUniformLocation( mPhongShaderProgram->getProgram(), "textureSampler");
}

void RenderWindow::calcAverageNormals(unsigned int* indices, unsigned int indexCount, GLfloat* vertices, unsigned int vertexCount,
    unsigned int vLength, unsigned int normalOffset)
{
    for (size_t i = 0; i < indexCount; i += 3)
    {
        unsigned int in0 = indices[i] * vLength;
        unsigned int in1 = indices[i + 1] * vLength;
        unsigned int in2 = indices[i + 2] * vLength;

        glm::vec3 v1(vertices[in1] - vertices[in0],
                     vertices[in1 + 1] - vertices[in0 + 1],
                     vertices[in1 + 2] - vertices[in0 + 2]);

        glm::vec3 v2(vertices[in2] - vertices[in0],
                    vertices[in2 + 1] - vertices[in0 + 1],
                    vertices[in2 + 2] - vertices[in0 + 2]);

        glm::vec3 normal = glm::cross(v1, v2);
        normal = glm::normalize(normal);

        in0 += normalOffset; in1 += normalOffset; in2 += normalOffset;
        vertices[in0] += normal.x; vertices[in0 + 1] += normal.y; vertices[in0 + 2] += normal.z;
        vertices[in1] += normal.x; vertices[in1 + 1] += normal.y; vertices[in1 + 2] += normal.z;
        vertices[in2] += normal.x; vertices[in2 + 1] += normal.y; vertices[in2 + 2] += normal.z;
    }

    for (size_t i = 0; i < vertexCount / vLength; i++)
    {
        unsigned int nOffset = i * vLength + normalOffset;
        glm::vec3 vec(vertices[nOffset], vertices[nOffset + 1], vertices[nOffset + 2]);
        vec = glm::normalize(vec);
        vertices[nOffset] = vec.x; vertices[nOffset + 1] = vec.y; vertices[nOffset + 2] = vec.z;
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

    if(event->key() == Qt::Key_Q)
    {
        mPathOne = !mPathOne;
    }

    if(event->key() == Qt::Key_C)
    {
        mCollision = !mCollision;
    }

    //You get the keyboard input like this
    if(event->key() == Qt::Key_A)
    {
        moveMiaX(-0.05f);
    }
    if(event->key() == Qt::Key_D)
    {
        moveMiaX(0.05f);
    }
    if(event->key() == Qt::Key_W)
    {
        moveMiaY(0.05f);
    }
    if(event->key() == Qt::Key_S)
    {
        moveMiaY(-0.05f);
    }

    std::cout << "WorldPos: \n";
    std::cout << "x: " << miaCollision->getPosition().x() << " y: " << miaCollision->getPosition().y() << " z: " << miaCollision->getPosition().z() << "\n";
}

void RenderWindow::moveMiaX(float movespeed)
{
    movespeed = (movespeed * (1/miaCollision->getRadius()));
    miaCollision->move(movespeed, 0.0f, 0.0f);
    miaCollision->mWorldPosition += QVector3D{movespeed * miaCollision->getRadius(), 0.0f, 0.0f};
    //mMap["mia"]->move(movespeed * miaCollision->getRadius(), 0.0f, 0.0f);
    mia->move(movespeed * miaCollision->getRadius(), 0.0f, 0.0f);
}

void RenderWindow::moveMiaY(float movespeed)
{
    movespeed = (movespeed * (1/miaCollision->getRadius()));
    miaCollision->move(0.0f, movespeed, 0.0f);
    miaCollision->mWorldPosition += QVector3D{0.0f, movespeed * miaCollision->getRadius(), 0.0f};
    //mMap["mia"]->move(0.0f, movespeed * miaCollision->getRadius(), 0.0f);
    mia->move(0.0f, movespeed * miaCollision->getRadius(), 0.0f);
}

void RenderWindow::ToggleCollision()
{
    if (mCollision == false)
    {
        for (int i = 0; i < mItems.size(); i++)
            mItems[i]->bShouldRender = false;

        miaCollision->bShouldRender = false;
    }
    else
    {
        for (int i = 0; i < mItems.size(); i++)
            mItems[i]->bShouldRender = true;

        miaCollision->bShouldRender = true;
    }
}

void RenderWindow::TogglePath()
{
    if (mPathOne)
    {
        BOT->bShouldRender = false;
        BOT2->bShouldRender = true;
        Path->bShouldRender = false;
        Path2->bShouldRender = true;
    }
    else
    {
        BOT->bShouldRender = true;
        BOT2->bShouldRender = false;
        Path2->bShouldRender = false;
        Path->bShouldRender = true;
    }
}
