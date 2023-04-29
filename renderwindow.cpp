#define STB_IMAGE_IMPLEMENTATION

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
#include <stdio.h>

#include "qthread.h"

#include <QLibrary>
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

    house=new House();

    mMap.insert(MapPair{"Object", new Tetrahedron(-7.5, 0.2, 0.3, 1.2)});

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
    mItems.push_back(new CollisionVolume(1.25, -5, 0, 2));

    // Oppgave 2 OBLIG 2
    mMap.insert(std::pair<std::string, VisualObject*>{"P1", new Tetrahedron(-3,-3, 0, 0.5)});
    mMap.insert(std::pair<std::string, VisualObject*>{"P2", new Tetrahedron(-2, 2, 0, 0.5)});
    mMap.insert(std::pair<std::string, VisualObject*>{"P3", new Tetrahedron( 2,-2, 0, 0.5)});
    mMap.insert(std::pair<std::string, VisualObject*>{"P4", new Tetrahedron( 3, 3, 0, 0.5)});

    BOT = new NPC("curve.txt");
    BOT2 = new NPC("curve2.txt");
    mMap.insert(MapPair{"NPC", BOT});
    mMap.insert(MapPair{"NPC2", BOT2});
    //triangulation = new Triangulation();

    mia = new InteractiveObject;
    miaCollision = new InteractiveCollisionVolume(1);
    //mMap.insert(MapPair{"mia", mia});
    mMap.insert(MapPair{"miaCollision", miaCollision});

    mModels.push_back(new ObjMesh("../3Dprog22/Objects/Tree/Tree_1.obj"));
    mModels.push_back(new ObjMesh("../3Dprog22/Objects/Tree/Tree_2.obj"));
    mModels.push_back(new ObjMesh("../3Dprog22/Objects/Tree/Tree_3.obj"));
    mModels.push_back(new ObjMesh("../3Dprog22/Objects/Log/Log_5.obj"));

    cat = new ObjMesh("../3Dprog22/Objects/cat.obj");
}

RenderWindow::~RenderWindow()
{
    //cleans up the GPU memory
    glDeleteVertexArrays( 1, &mVAO );
    glDeleteBuffers( 1, &mVBO );
}

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

    // Calculate normals
    mia->calcAverageNormalsSmooth();
    house->calcAverageNormalsSmooth();

    //Compile shaders:
    //NB: hardcoded path to files! You have to change this if you change directories for the project.
    //Qt makes a build-folder besides the project folder. That is why we go down one directory
    // (out of the build-folder) and then up into the project folder.
    mPlainShaderProgram = new Shader( "../3Dprog22/plainshader.vert",
                                    "../3Dprog22/plainshader.frag");

    mTexShaderProgram = new Shader( "../3Dprog22/texshader.vert",
                                    "../3Dprog22/texshader.frag");

    mPhongShaderProgram = new Shader( "../3Dprog22/phongshader.vert",
                                    "../3Dprog22/phongshader.frag");

    setupPlainShader();
    setupTextureShader();
    setupPhongShader();

    heightMap = new HeightMap((char*)("../3Dprog22/HeightMaps/hMap3.png"));
    heightMap->LoadHeightMap();

    brickTexture = new Texture((char*)("../3Dprog22/Textures/brick.png"));
    brickTexture->LoadTextureA();
    dirtTexture = new Texture((char*)("../3Dprog22/Textures/dirt.png"));
    dirtTexture->LoadTextureA();
    plainTexture = new Texture((char*)("../3Dprog22/Textures/plain.png"));
    plainTexture->LoadTextureA();
    woodTexture = new Texture((char*)("../3Dprog22/Textures/wood.png"));
    woodTexture->LoadTexture();
    crateTexture = new Texture((char*)("../3Dprog22/Textures/woodencrate.png"));
    crateTexture->LoadTextureA();
    catTexture = new Texture((char*)("../3Dprog22/Textures/cat.png"));
    catTexture->LoadTextureA();


    shinyMaterial = new Material(4.0f, 256);
    dullMaterial = new Material(0.3f, 4);

    mainLight = new DirectionalLight(0.8f, 0.4f, 0.2f,      //rgb
                                    0.5f, 0.5,              //ambientIntensity, specularIntensity
                                    0.0f, 0.0f, -1.0f);     //xyz (directions)

    mPointLights.push_back( new PointLight(1.0f, 0.5f, 0.0f,
                                    0.2f, 0.25f,
                                    -5.0f, 5.0f, 3.0f,
                                    0.2f, 0.1f, 0.1f));
    PointLightCount++;
    mPointLights.push_back( new PointLight(1.0f, 0.0f, 1.0f,    //rgb
                                    0.1f, 1.0f,                 //ambientIntensity, diffuseIntensity
                                    2.0f, 2.0f, 3.0f,           //xyz
                                    0.5f, 0.2f, 0.1f));         //const, lin, exp
    PointLightCount++;
    mPointLights.push_back( new PointLight(0.0f, 0.0f, 1.0f,
                                    0.1f, 1.0f,
                                    -1.0f, 5.0f, 3.0f,
                                    0.2f, 0.1f, 0.1f));
    PointLightCount++;

    // order: rt, lf, up, dn, bk, ft.
    std::vector<std::string> skyboxFaces;

    skyboxFaces.push_back("../3Dprog22/Textures/Skybox/cupertin-lake_rt.tga");
    skyboxFaces.push_back("../3Dprog22/Textures/Skybox/cupertin-lake_lf.tga");
    skyboxFaces.push_back("../3Dprog22/Textures/Skybox/cupertin-lake_up.tga");
    skyboxFaces.push_back("../3Dprog22/Textures/Skybox/cupertin-lake_dn.tga");
    skyboxFaces.push_back("../3Dprog22/Textures/Skybox/cupertin-lake_bk.tga");
    skyboxFaces.push_back("../3Dprog22/Textures/Skybox/cupertin-lake_ft.tga");

    skybox = new Skybox(skyboxFaces);

    //mCamera.init(mPmatrixUniform, mVmatrixUniform);

    // Pickups
    for (auto it=mObjects.begin(); it!=mObjects.end(); it++)
        (*it)->init(mMmatrixUniform0);

    // Kollisjonsvolum
    for (auto it=mItems.begin(); it!=mItems.end(); it++)
        (*it)->init(mMmatrixUniform0);

    // Alternativ: Erstatter std::vector<VisualObject*> med unordered map
    for (auto it=mMap.begin(); it!=mMap.end(); it++)
        (*it).second->init(mMmatrixUniform0);

    // .obj modeller
    for (auto it=mModels.begin(); it!=mModels.end(); it++)
    {
        (*it)->init(mUniformModel);
        (*it)->mMatrix.translate(0, 7, -0.5);
        (*it)->mMatrix.rotate(90, 1, 0, 0);
    }
    // Trees
    mModels[0]->mMatrix.translate(6,-0,-1);
    mModels[1]->mMatrix.translate(-7,-0,-1);

    // Log
    mModels[3]->mMatrix.translate(-4, 0.6, 2);
    mModels[3]->mMatrix.rotate(45, 0,1,0);

    // Cat
    cat->mMatrix.translate(3,0,0);
    cat->mMatrix.rotate(90, 1.0 , 0.0, 0.0);
    cat->mMatrix.scale(0.5);

    // objects using phong shading
    //triangulation->init(mUniformModel);
    mia->init(mUniformModel);
    heightMap->init(mUniformModel);
    house->init(mUniformModel);
    cat->init(mUniformModel);
    glBindVertexArray(0);       //unbinds any VertexArray - good practice

    // Additional setup
    moveMiaX(0);
    moveMiaY(-3);

    house->move(-7.0, 0.0, 1.0);
    house->mMatrix.rotate(90, 1, 0, 0);

    BOT2->bShouldRender = false;
    Path2->bShouldRender = false;
}

// Called each frame - doing the rendering!!!
void RenderWindow::render()
{
    mCamera.init(mPmatrixUniform0, mVmatrixUniform0);
    mCamera.perspective(70.0f, 16.0f/9.0f, 0.1f, 20.f);

    mTimeStart.restart(); //restart FPS clock
    mContext->makeCurrent(this); //must be called every frame (every time mContext->swapBuffers is called)

    initializeOpenGLFunctions();    //must call this every frame it seems...

    //clear the screen for each redraw
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Make sure to call glClear BEFORE the Skybox
    skybox->DrawSkybox(mCamera.mVmatrix, mCamera.mPmatrix);

    QVector3D followCamera = {mia->getPosition().x(),
                              mia->getPosition().y() - 3,
                              2};
    QVector3D playerPos = {mia->getPosition().x(),
                           mia->getPosition().y(),
                           1};

    if (bSceneOne)
        // Scene 1
        mCamera.lookAt(followCamera, playerPos , QVector3D{0,1,0} );
    else
        // Scene 2
        mCamera.lookAt( QVector3D{-7,-1,2}, QVector3D{-7,0,0}, QVector3D{0,1,0} );

    //what shader to use (plain shader)
    glUseProgram(mPlainShaderProgram->getProgram());
    glUniformMatrix4fv(mVmatrixUniform0, 1, GL_TRUE, mCamera.mVmatrix.constData());
    glUniformMatrix4fv(mPmatrixUniform0, 1, GL_TRUE, mCamera.mPmatrix.constData());
    mCamera.update();

    for (auto it=mMap.begin(); it!=mMap.end(); it++)
        (*it).second->draw();

    for (auto it=mItems.begin(); it!=mItems.end(); it++)
        (*it)->draw();

    for (auto it = mObjects.begin(); it != mObjects.end(); it++)
        (*it)->draw();

    cat->mMatrix.rotate(1.0, 0.0 , 1.0, 0.0);


    // what shader to use (phong shader)
    glUseProgram(mPhongShaderProgram->getProgram());
    glUniformMatrix4fv(mUniformView, 1, GL_TRUE, mCamera.mVmatrix.constData());
    glUniformMatrix4fv(mUniformProjection, 1, GL_TRUE, mCamera.mPmatrix.constData());
    glUniform3f(mUniformEyePosition, mCamera.getCameraPosition().x, mCamera.getCameraPosition().y, mCamera.getCameraPosition().z);
    //checkForGLerrors();
    //Additional parameters for light shader:
    SetDirectionalLight(mainLight);
    SetPointLights();
    //Update Camera
    mCamera.update();

    // mia
    crateTexture->UseTexture();
    shinyMaterial->UseMaterial(mUniformSpecularIntensity, mUniformShininess);
    glUniform1i(mTextureUniform, 1);
    mia->draw();

    // surface obj
    woodTexture->UseTexture();
    dullMaterial->UseMaterial(mUniformSpecularIntensity, mUniformShininess);
    glUniform1i(mTextureUniform, 1);
    //triangulation->draw();

    // hmap
    dirtTexture->UseTexture();
    shinyMaterial->UseMaterial(mUniformSpecularIntensity, mUniformShininess);
    glUniform1i(mTextureUniform, 1);
    heightMap->draw();

    // house obj
    brickTexture->UseTexture();
    dullMaterial->UseMaterial(mUniformSpecularIntensity, mUniformShininess);
    glUniform1i(mTextureUniform, 1);
    house->draw();

    plainTexture->UseTexture();
    shinyMaterial->UseMaterial(mUniformSpecularIntensity, mUniformShininess);
    for (auto it = mModels.begin(); it != mModels.end(); it++)
        (*it)->draw();

    catTexture->UseTexture();
    dullMaterial->UseMaterial(mUniformSpecularIntensity, mUniformShininess);
    cat->draw();

    //mMap["disc"]->move(0.05f);

    //Calculate framerate before
    // checkForGLerrors() because that call takes a long time
    // and before swapBuffers(), else it will show the vsync time
    calculateFramerate();

    //using our expanded OpenGL debugger to check if everything is OK.
    checkForGLerrors();

    // Qt require us to call this swapBuffers() -function.
    // swapInterval is 1 by default which means that swapBuffers() will (hopefully) block
    // and wait for vsync.
    mContext->swapBuffers(this);

    mPointLights[0]->setPos(mia->getPosition().x(),
                            mia->getPosition().y(),
                            mia->getPosition().z() + 1.5);

    CollisionHandling();
    ToggleCollision();
    TogglePath();
    ToggleWireframe();
}

void RenderWindow::CollisionHandling()
{
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
        QVector3D dist = miaCollision->getPosition() - house->getPosition();
        float d = dist.length();

        float r1 = miaCollision->getRadius(); // Må legges til
        float r2 =house->getRadius();

        if (d < r1 + r2)
        {
            bSceneOne = false;
        }
        else
        {
            bSceneOne = true;
        }
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
    mUniformProjection = glGetUniformLocation(mPhongShaderProgram->getProgram(), "projection");
    mUniformModel = glGetUniformLocation(mPhongShaderProgram->getProgram(), "model");
    mUniformView = glGetUniformLocation(mPhongShaderProgram->getProgram(), "view");
    mTextureUniform = glGetUniformLocation(mTexShaderProgram->getProgram(), "theTexture");

    uniformDirectionalLight.uniformAmbientIntensity = glGetUniformLocation(mPhongShaderProgram->getProgram(), "directionalLight.base.ambientIntensity");
    uniformDirectionalLight.uniformColor = glGetUniformLocation(mPhongShaderProgram->getProgram(), "directionalLight.base.color");
    uniformDirectionalLight.uniformDirection = glGetUniformLocation(mPhongShaderProgram->getProgram(), "directionalLight.direction");
    uniformDirectionalLight.uniformDiffuseIntensity = glGetUniformLocation(mPhongShaderProgram->getProgram(), "directionalLight.base.diffuseIntensity");
    mUniformSpecularIntensity = glGetUniformLocation(mPhongShaderProgram->getProgram(), "material.specularIntensity");
    mUniformShininess = glGetUniformLocation(mPhongShaderProgram->getProgram(),"material.shininess");

    mUniformEyePosition = glGetUniformLocation(mPhongShaderProgram->getProgram(), "eyePosition");

    uniformPointLightCount = glGetUniformLocation(mPhongShaderProgram->getProgram(), "pointLightCount");

    for (size_t i = 0; i < MAX_POINT_LIGHTS; i++)
    {
        char locBuff[100] = { '\0' };

        snprintf(locBuff, sizeof(locBuff), "pointLights[%d].base.color", i);
        uniformPointLight[i].uniformColor = glGetUniformLocation(mPhongShaderProgram->getProgram(), locBuff);

        snprintf(locBuff, sizeof(locBuff), "pointLights[%d].base.ambientIntensity", i);
        uniformPointLight[i].uniformAmbientIntensity = glGetUniformLocation(mPhongShaderProgram->getProgram(), locBuff);

        snprintf(locBuff, sizeof(locBuff), "pointLights[%d].base.diffuseIntensity", i);
        uniformPointLight[i].uniformDiffuseIntensity = glGetUniformLocation(mPhongShaderProgram->getProgram(), locBuff);

        snprintf(locBuff, sizeof(locBuff), "pointLights[%d].position", i);
        uniformPointLight[i].uniformPosition = glGetUniformLocation(mPhongShaderProgram->getProgram(), locBuff);

        snprintf(locBuff, sizeof(locBuff), "pointLights[%d].constant", i);
        uniformPointLight[i].uniformConstant = glGetUniformLocation(mPhongShaderProgram->getProgram(), locBuff);

        snprintf(locBuff, sizeof(locBuff), "pointLights[%d].linear", i);
        uniformPointLight[i].uniformLinear = glGetUniformLocation(mPhongShaderProgram->getProgram(), locBuff);

        snprintf(locBuff, sizeof(locBuff), "pointLights[%d].exponent", i);
        uniformPointLight[i].uniformExponent = glGetUniformLocation(mPhongShaderProgram->getProgram(), locBuff);
    }
}

void RenderWindow::SetDirectionalLight(DirectionalLight *dLight)
{
    dLight->UseLight(uniformDirectionalLight.uniformAmbientIntensity, uniformDirectionalLight.uniformColor,
                     uniformDirectionalLight.uniformDiffuseIntensity, uniformDirectionalLight.uniformDirection);
}

void RenderWindow::SetPointLights()
{
    if (PointLightCount > MAX_POINT_LIGHTS) PointLightCount = MAX_POINT_LIGHTS;

    glUniform1i(uniformPointLightCount, PointLightCount);

    for (size_t i = 0; i < PointLightCount; i++)
    {
        mPointLights[i]->UseLight(uniformPointLight[i].uniformAmbientIntensity,
                                uniformPointLight[i].uniformColor,
                                uniformPointLight[i].uniformDiffuseIntensity,
                                uniformPointLight[i].uniformPosition,
                                uniformPointLight[i].uniformConstant,
                                uniformPointLight[i].uniformLinear,
                                uniformPointLight[i].uniformExponent);
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

    if(event->key() == Qt::Key_V)
    {
        mWireframe = !mWireframe;
    }

    //You get the keyboard input like this
    float moveSpeed = 0.06;
    if(event->key() == Qt::Key_A)
    {
        if(mia!=nullptr)
            moveMiaX(-moveSpeed);
    }
    if(event->key() == Qt::Key_D)
    {
        if(mia!=nullptr)
            moveMiaX(moveSpeed);
    }
    if(event->key() == Qt::Key_W)
    {
        if(mia!=nullptr)
            moveMiaY(moveSpeed);
    }
    if(event->key() == Qt::Key_S)
    {
        if(mia!=nullptr)
            moveMiaY(-moveSpeed);
    }

    std::cout << "WorldPos: \n";
    std::cout << "x: " << miaCollision->getPosition().x() << " y: " << mia->getPosition().y() << " z: " << mia->getPosition().z() << "\n";
}

void RenderWindow::moveMiaX(float movespeed)
{
    glm::vec3 playerpos = glm::vec3(mia->getPosition().x(), mia->getPosition().y(), mia->getPosition().z());
    float height = heightMap->GetSurfaceHeight(playerpos) - miaCollision->getPosition().z();

    //movespeed = (movespeed * ));

    miaCollision->move(movespeed*(1/miaCollision->getRadius()), 0.0f, height*1/miaCollision->getRadius());


    miaCollision->mWorldPosition += QVector3D{movespeed, 0.0f, height};

    mia->move(movespeed, 0.0f, height);
    mia->mWorldPosition += QVector3D{movespeed, 0.0f, height};
}

void RenderWindow::moveMiaY(float movespeed)
{
    glm::vec3 playerpos = glm::vec3(mia->getPosition().x(), mia->getPosition().y(), mia->getPosition().z());
    float height = heightMap->GetSurfaceHeight(playerpos) - miaCollision->getPosition().z();


    miaCollision->move(0.0f, movespeed * (1/miaCollision->getRadius()), height*1/miaCollision->getRadius());
    miaCollision->mWorldPosition += QVector3D{0.0f, movespeed, height};

    mia->move(0.0f,movespeed,height);
    mia->mWorldPosition += QVector3D{0.0f, movespeed, height};
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

void RenderWindow::ToggleWireframe()
{
    if(mWireframe)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);    //turn on wireframe mode
    else
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);    //turn off wireframe mode
}





