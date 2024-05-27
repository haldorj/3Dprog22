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

#include <QApplication>
#include <QProcess>

// Shader Paths
constexpr char PLAIN_SHADER_VERTEX_PATH[]   = "../3Dprog22/plainshader.vert";
constexpr char PLAIN_SHADER_FRAGMENT_PATH[] = "../3Dprog22/plainshader.frag";
constexpr char TEXTURE_SHADER_VERTEX_PATH[]   =  "../3Dprog22/texshader.vert";
constexpr char TEXTURE_SHADER_FRAGMENT_PATH[] = "../3Dprog22/texshader.frag";
constexpr char PHONG_SHADER_VERTEX_PATH[] =  "../3Dprog22/phongshader.vert";
constexpr char PHONG_SHADER_FRAGMENT_PATH[] = "../3Dprog22/phongshader.frag";

// Texture Paths
constexpr char TEXTURE_BRICK_PATH[] =  "../3Dprog22/Textures/brick.png";
constexpr char TEXTURE_DIRT_PATH[]  =  "../3Dprog22/Textures/dirt.png";
constexpr char TEXTURE_PLAIN_PATH[] =  "../3Dprog22/Textures/plain.png";
constexpr char TEXTURE_WOOD_PATH[]  =  "../3Dprog22/Textures/wood.png";
constexpr char TEXTURE_CRATE_PATH[] =  "../3Dprog22/Textures/woodencrate.png";
constexpr char TEXTURE_CAT_PATH[]   =  "../3Dprog22/Textures/cat.png";

// Model paths
constexpr char MODELS_TREE_1_PATH[] = "../3Dprog22/Objects/Tree/Tree_1.obj";
constexpr char MODELS_TREE_2_PATH[] = "../3Dprog22/Objects/Tree/Tree_2.obj";
constexpr char MODELS_TREE_3_PATH[] = "../3Dprog22/Objects/Tree/Tree_3.obj";
constexpr char MODELS_LOG_PATH[] =    "../3Dprog22/Objects/Log/Log_5.obj";
constexpr char MODELS_CAT_PATH[] =    "../3Dprog22/Objects/cat.obj";

// Heightmap Path
constexpr char HEIGHTMAP_PATH[] =    "../3Dprog22/HeightMaps/hMap3.png";

// Skybox Path

constexpr char SKYBOX_RT_PATH[] = "../3Dprog22/Textures/Skybox/cupertin-lake_rt.tga";
constexpr char SKYBOX_LF_PATH[] = "../3Dprog22/Textures/Skybox/cupertin-lake_lf.tga";
constexpr char SKYBOX_UP_PATH[] = "../3Dprog22/Textures/Skybox/cupertin-lake_up.tga";
constexpr char SKYBOX_DN_PATH[] = "../3Dprog22/Textures/Skybox/cupertin-lake_dn.tga";
constexpr char SKYBOX_BK_PATH[] = "../3Dprog22/Textures/Skybox/cupertin-lake_bk.tga";
constexpr char SKYBOX_FT_PATH[] = "../3Dprog22/Textures/Skybox/cupertin-lake_ft.tga";

// Paths for Uniforms
constexpr char UNIFORM_PROJECTION_MATRIX_PATH[] = "pmatrix";
constexpr char UNIFORM_VIEW_MATRIX_PATH[] = "vmatrix";
constexpr char UNIFORM_MATRIX_MODEL_PATH[] = "matrix";
constexpr char UNIFORM_TEXTURE_PATH[] = "textureSampler";

// paths for phong
constexpr char PHONG_PROJECTION_PATH[] = "projection";
constexpr char PHONG_MODEL_PATH[] = "model";
constexpr char PHONG_VIEW_PATH[] = "view";
constexpr char PHONG_TEXTURE_PATH[] = "theTexture";

// Paths for directional light
constexpr char DIRECTIONALLIGHT_AMBIENT_INTENSITY[] = "directionalLight.base.ambientIntensity";
constexpr char DIRECTIONALLIGHT_BASE_COLOR[] = "directionalLight.base.color";
constexpr char DIRECTIONALLIGHT_DIRECTION[] = "directionalLight.direction";
constexpr char DIRECTIONALLIGHT_BASE_DIFFUSE_INTENSITY[] = "directionalLight.base.diffuseIntensity";

constexpr char PHONG_MATERIAL_SPECULAR_INTENSITY[] = "material.specularIntensity";
constexpr char PHONG_MATERIAL_SHININESS[] = "material.shininess";
constexpr char PHONG_MATERIAL_EYE_POSITION[] = "eyePosition";

// Path for point light
constexpr char POINTLIGHT_COUNT[] = "pointLightCount";



/*
    23DPRO101 3D-programmering
    - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    This project was a collaboration up until the day of the exam.
    Between candidates:
    1210, 1216, 1219.

    - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            P R O J E C T   C O N T R O L S
    - - - - - - - - - - - - - - - - - - - - - - - - - - - -
            WASD - move the interactive object
            L/R ARROW KEYS - Camera controls

            Q - Toggle NPC path
            C - Toggle Collision volumes
            V - Toggle wireframe

            SPACE - Toggle first person / third person mode
            L - Toggle light source (cubeLight) on and off

            1 - cat with phong shader
            2 - cat with unlit texture shader
            3 - cat with plain shader
*/

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
    door = new Plane();
    door->GetMatrix().translate(-4.99f, -0.5f, -0.4f);
    mMap.insert(MapPair{"door", door});
    //mObjects.push_back(door);

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
    doorCollision = new CollisionVolume(1.25, -5, 0, 2);
    mMap.insert(MapPair{"doorCollison", doorCollision});
    //mItems.push_back(new CollisionVolume(1.25, -5, 0, 2));

    // Oppgave 2 OBLIG 2
    mMap.insert(MapPair{"P1", new Tetrahedron(-3,-3, 0, 0.5)});
    mMap.insert(MapPair{"P2", new Tetrahedron(-2, 2, 0, 0.5)});
    mMap.insert(MapPair{"P3", new Tetrahedron( 2,-2, 0, 0.5)});
    mMap.insert(MapPair{"P4", new Tetrahedron( 3, 3, 0, 0.5)});

    BOT = new NPC("curve.txt");
    BOT2 = new NPC("curve2.txt");
    mMap.insert(MapPair{"NPC", BOT});
    mMap.insert(MapPair{"NPC2", BOT2});
    //triangulation = new Triangulation();

    mia = new InteractiveObject;
    miaCollision = new InteractiveCollisionVolume(1);
    //mMap.insert(MapPair{"mia", mia});
    mMap.insert(MapPair{"miaCollision", miaCollision});

    // ObjMesh -> objects in the scene with a mesh reading from an obj file.
    mModels.push_back(new ObjMesh(MODELS_TREE_1_PATH));
    mModels.push_back(new ObjMesh(MODELS_TREE_2_PATH));
    mModels.push_back(new ObjMesh(MODELS_TREE_3_PATH));
    mModels.push_back(new ObjMesh(MODELS_LOG_PATH));

    // lol damn, we actually have 3 objects ontop of each other :oo?
    for (int i = 0; i < 3; i++)
        cats[i] = new ObjMesh(MODELS_CAT_PATH);

    cubeLight = new Cube(1, 0, 2, 0, 1, 0, 1);
    mMap.insert(MapPair{"CubeLight", cubeLight});
    cubeLightOff = new Cube(1, 0, 2, 0, 0.5, 0.5, 0.5);
    mMap.insert(MapPair{"CubeLightOff", cubeLightOff});
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
    mPlainShaderProgram = new Shader( PLAIN_SHADER_VERTEX_PATH,
                                      PLAIN_SHADER_FRAGMENT_PATH);

    mTexShaderProgram = new Shader( TEXTURE_SHADER_VERTEX_PATH,
                                    TEXTURE_SHADER_FRAGMENT_PATH);

    mPhongShaderProgram = new Shader(PHONG_SHADER_VERTEX_PATH,
                                     PHONG_SHADER_FRAGMENT_PATH);


    setupPlainShader();
    setupTextureShader();
    setupPhongShader();

    heightMap = new HeightMap((char*)(HEIGHTMAP_PATH));
    heightMap->LoadHeightMap();

    // 1. Create new texture
    TextureStruct.brickTexture = new Texture((char*)(TEXTURE_BRICK_PATH));
    TextureStruct.brickTexture->LoadTextureA();
    TextureStruct.dirtTexture = new Texture((char*)(TEXTURE_DIRT_PATH));
    TextureStruct.dirtTexture->LoadTextureA();
    TextureStruct.plainTexture = new Texture((char*)(TEXTURE_PLAIN_PATH));
    TextureStruct.plainTexture->LoadTextureA();
    TextureStruct.woodTexture = new Texture((char*)(TEXTURE_WOOD_PATH));
    TextureStruct.woodTexture->LoadTexture();
    TextureStruct.crateTexture = new Texture((char*)(TEXTURE_CRATE_PATH));
    TextureStruct.crateTexture->LoadTextureA();
    TextureStruct.catTexture = new Texture((char*)(TEXTURE_CAT_PATH));
    TextureStruct.catTexture->LoadTextureA();


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

    skyboxFaces.push_back(SKYBOX_RT_PATH);
    skyboxFaces.push_back(SKYBOX_LF_PATH);
    skyboxFaces.push_back(SKYBOX_UP_PATH);
    skyboxFaces.push_back(SKYBOX_DN_PATH);
    skyboxFaces.push_back(SKYBOX_BK_PATH);
    skyboxFaces.push_back(SKYBOX_FT_PATH);

    skybox = new Skybox(skyboxFaces);

    //mCamera.init(mPmatrixUniform, mVmatrixUniform);

    // Pickups
    for (auto it=mObjects.begin(); it!=mObjects.end(); it++)
        (*it)->init(PlainShader.mMmatrixUniform0);

    // Kollisjonsvolum
    for (auto it=mItems.begin(); it!=mItems.end(); it++)
        (*it)->init(PlainShader.mMmatrixUniform0);

    // Alternativ: Erstatter std::vector<VisualObject*> med unordered map
    for (auto it=mMap.begin(); it!=mMap.end(); it++)
        (*it).second->init(PlainShader.mMmatrixUniform0);

    // .obj modeller
    for (auto it=mModels.begin(); it!=mModels.end(); it++)
    {
        (*it)->init(PhongShader.mUniformModel);
        (*it)->GetMatrix().translate(0, 7, -0.5);
        (*it)->GetMatrix().rotate(90, 1, 0, 0);
    }
    // Trees
    mModels[0]->GetMatrix().translate(6,-0,-1);
    mModels[1]->GetMatrix().translate(-7,-0,-1);

    // Log
    mModels[3]->GetMatrix().translate(-4, 0.6, 2);
    mModels[3]->GetMatrix().rotate(45, 0,1,0);

    // Cat
    for (const auto &cat : cats)
    {
        cat->GetMatrix().translate(3,0,0);
        cat->GetMatrix().rotate(90, 1.0 , 0.0, 0.0);
        cat->GetMatrix().scale(0.5);
    }


    // 2. init with correct uniform model

    // objects using phong shading
    //triangulation->init(mUniformModel);
    mia->init(PhongShader.mUniformModel);
    heightMap->init(PhongShader.mUniformModel);
    house->init(PhongShader.mUniformModel);
    cats[2]->init(PlainShader.mMmatrixUniform0);
    cats[1]->init(TextureShader.mMmatrixUniform1);
    cats[0]->init(PhongShader.mUniformModel);
    glBindVertexArray(0);       //unbinds any VertexArray - good practice

    // Additional setup
    moveMiaX(0);
    moveMiaY(-3);

    house->GetMatrix().translate(-7.0, 0.0, 1.0);
    // house->GetWorldPosition() = QVector3D{-7.0, 0.0, 1.0};
    house->SetWorldPosition(-7.0, 0.0, 1.0);
    house->GetMatrix().rotate(90, 1, 0, 0);

    BOT2->bShouldRender = false;
    Path2->bShouldRender = false;

    // Camera
    mCamera.rotateAroundTarget(mia->GetWorldPosition(), 0.0, -90);

    catShader = CatShader::PhongShader;
}

// Called each frame - doing the rendering!!!
void RenderWindow::render()
{
    mCamera.init(PlainShader.mPmatrixUniform0, PlainShader.mVmatrixUniform0);
    mCamera.perspective(70.0f, 16.0f/9.0f, 0.1f, 20.f);

    mTimeStart.restart(); //restart FPS clock
    mContext->makeCurrent(this); //must be called every frame (every time mContext->swapBuffers is called)

    initializeOpenGLFunctions();    //must call this every frame it seems...

    //clear the screen for each redraw
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    followCamera = {mia->getPosition().x(),
                  mia->getPosition().y() - 3,
                  2};
    playerPos = {mia->getPosition().x(),
               mia->getPosition().y(),
               mia->getPosition().z() + 1};

    // Trigger that turns light on and off
    if (!bCubeLight)
    {
        mPointLights[1]->setLightColor(0.0f,0.0f,0.0f,0.0f,0.0f);
        cubeLight->bShouldRender = false;
        cubeLightOff->bShouldRender = true;
    }
    else
    {
        mPointLights[1]->setLightColor(1.0f, 0.0f, 1.0f, 0.1f, 1.0f);
        cubeLight->bShouldRender = true;
        cubeLightOff->bShouldRender = false;
    }


    // Switch between Third/First person camera
    if (bSceneOne)
    {
        // Scene 1
        if (!bFirstPerson)
            // Third person
            mCamera.rotateAroundTarget(mia->GetWorldPosition(), 0.0, rotation);
        else
            // First person
            mCamera.lookAt(playerPos, QVector3D{playerPos.x(), 1000, 0}, QVector3D{0,1,0} );

        rotation = 0;
    }
    else
    {
        // Scene 2
        if (!bFirstPerson)
            // Third person
            mCamera.lookAt( QVector3D{-7,-1,2}, QVector3D{-7,0,0}, QVector3D{0,1,0} );
        else
            // First person
            mCamera.lookAt(playerPos, QVector3D{playerPos.x(), 1000, 0}, QVector3D{0,1,0} );
    }

    // Make sure to call glClear BEFORE the Skybox
    skybox->DrawSkybox(mCamera.GetViewMatrix(), mCamera.GetPerspectiveMatrix());

    //what shader to use (plain shader)
    glUseProgram(mPlainShaderProgram->getProgram());
    glUniformMatrix4fv(PlainShader.mVmatrixUniform0, 1, GL_TRUE, mCamera.GetViewMatrixData());
    glUniformMatrix4fv(PlainShader.mPmatrixUniform0, 1, GL_TRUE, mCamera.GetPerspectiveMatrixData());
    mCamera.update();

    for (auto it=mMap.begin(); it!=mMap.end(); it++)
        (*it).second->draw();

    for (auto it=mItems.begin(); it!=mItems.end(); it++)
        (*it)->draw();

    for (auto it = mObjects.begin(); it != mObjects.end(); it++)
        (*it)->draw();

    if(catShader == CatShader::PlainShader)
        cats[2]->draw();

    //what shader to use (unlit texture shader)
    glUseProgram(mTexShaderProgram->getProgram());
    glUniformMatrix4fv(TextureShader.mVmatrixUniform1, 1, GL_FALSE, mCamera.GetViewMatrixData());
    glUniformMatrix4fv(TextureShader.mPmatrixUniform1, 1, GL_FALSE, mCamera.GetPerspectiveMatrixData());
    mCamera.update();

    if(catShader == CatShader::UnlitTextureShader)
    {
        //glUniform1i(mTextureUniform1, 1);
        cats[1]->draw();
    }

    // what shader to use (phong shader)
    glUseProgram(mPhongShaderProgram->getProgram());
    glUniformMatrix4fv(PhongShader.mUniformView, 1, GL_FALSE, mCamera.GetViewMatrixData());
    glUniformMatrix4fv(PhongShader.mUniformProjection, 1, GL_FALSE, mCamera.GetPerspectiveMatrixData());
    glUniform3f(PhongShader.mUniformEyePosition, mCamera.getCameraPosition().x, mCamera.getCameraPosition().y, mCamera.getCameraPosition().z);
    //checkForGLerrors();
    //Additional parameters for light shader:
    SetDirectionalLight(mainLight);
    SetPointLights();
    //Update Camera
    mCamera.update();

    if(catShader == CatShader::PhongShader)
        cats[0]->draw();

    // 3. use textures

    // mia
    TextureStruct.crateTexture->UseTexture();
    shinyMaterial->UseMaterial(PhongShader.mUniformSpecularIntensity, PhongShader.mUniformShininess);
    glUniform1i(PhongShader.mTextureUniform, 1);
    mia->draw();

    // surface obj
    TextureStruct.woodTexture->UseTexture();
    dullMaterial->UseMaterial(PhongShader.mUniformSpecularIntensity, PhongShader.mUniformShininess);
    glUniform1i(PhongShader.mTextureUniform, 1);
    //triangulation->draw();

    // hmap
    TextureStruct.dirtTexture->UseTexture();
    shinyMaterial->UseMaterial(PhongShader.mUniformSpecularIntensity, PhongShader.mUniformShininess);
    glUniform1i(PhongShader.mTextureUniform, 1);
    heightMap->draw();

    // house obj
    TextureStruct.brickTexture->UseTexture();
    dullMaterial->UseMaterial(PhongShader.mUniformSpecularIntensity, PhongShader.mUniformShininess);
    glUniform1i(PhongShader.mTextureUniform, 1);
    house->draw();


    TextureStruct.plainTexture->UseTexture();
    shinyMaterial->UseMaterial(PhongShader.mUniformSpecularIntensity, PhongShader.mUniformShininess);
    for (auto it = mModels.begin(); it != mModels.end(); it++)
        (*it)->draw();

    for (const auto &cat : cats)
        cat->GetMatrix().rotate(1.0, 0.0 , 1.0, 0.0);
    TextureStruct.catTexture->UseTexture();
    dullMaterial->UseMaterial(PhongShader.mUniformSpecularIntensity, PhongShader.mUniformShininess);

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


    // TODO: LOOK FOR ANOTHER WAY TO HANDLE THIS, MAYBE PUT IT INTO A BOOLEAN FUNCTION THAT RETURNS SOMETHING IN A IF STATEMENT
    if (miaCollision->GetWorldPosition().x() < -10)
        moveMiaX(0.03);

    if (miaCollision->GetWorldPosition().x() > 10)
        moveMiaX(-0.03);

    if ( miaCollision->GetWorldPosition().y() < -8)
        moveMiaY(0.03);

    if (miaCollision->GetWorldPosition().y() > 8)
        moveMiaY(-0.03);
}

void RenderWindow::CollisionHandling()
{
    // "Collision between player and pickups"
    for (auto i = 0; i < mItems.size(); i++)
    {
        // Distance check
        QVector3D dist = miaCollision->getPosition() - mItems[i]->getPosition();
        // [x1 – x2, y1 – y2, z1 – z2]
        float d = dist.length();
        //sqrt((x1 – x2)^2, (y1 – y2)^2, (z1 – z2)^2)
        float r1 = miaCollision->getRadius(); // Må legges til
        float r2 = mItems[i]->getRadius();

        if (d < r1 + r2 && mItems[i]->bIsActive == true)
        {
            // Move objects away
            mItems[i]->move(100,100,100);
            //When picking up trophies or pickups items, then count the score and print it.
            score++;
            mMainWindow->updateScore(score);
            std::cout << "You have picked up " << score << " items! \n";

            mItems[i]->bIsActive = false;
            if (mObjects[i])
            {
                mObjects[i]->bShouldRender = false;
                mObjects[i]->OpenDoor();
            }
        }
    }

    // door collision
    if (door && doorCollision)
    {
        QVector3D dist = miaCollision->getPosition() - doorCollision->getPosition();
        float d = dist.length();
        float r1 = miaCollision->getRadius();
        float r2 = doorCollision->getRadius();
        if (d < r1 + r2 && doorCollision->bIsActive == true)
        {
            doorCollision->bIsActive = false;
            doorCollision->move(100,100,100);

            door->OpenDoor();
            std::cout << "You opened the door! \n";
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

    // Hitting an enemy restarts the game.
    // Switch to 2nd NPC path to turn this feature off.

    if (BOT && !mPathOne)
    {
        //std::cout << "X: " << BOT->getPosition().x() << " Y: " << BOT->getPosition().y() << "\n";
        QVector3D dist = miaCollision->getPosition() - (BOT->GetWorldPosition());
        float d = dist.length();
        float r1 = miaCollision->getRadius();
        float r2 = 0.25;

        if (d < r1 + r2)
        {
            qApp->quit();
            QProcess::startDetached(qApp->arguments()[0], qApp->arguments());
        }
    }
}

void RenderWindow::setupPlainShader()
{
    PlainShader.mPmatrixUniform0 =  glGetUniformLocation( mPlainShaderProgram->getProgram(), UNIFORM_PROJECTION_MATRIX_PATH );
    PlainShader.mVmatrixUniform0 =  glGetUniformLocation( mPlainShaderProgram->getProgram(), UNIFORM_VIEW_MATRIX_PATH );
    PlainShader.mMmatrixUniform0 =  glGetUniformLocation( mPlainShaderProgram->getProgram(), UNIFORM_MATRIX_MODEL_PATH );
}

void RenderWindow::setupTextureShader()
{
    TextureShader.mPmatrixUniform1 =  glGetUniformLocation( mTexShaderProgram->getProgram(), UNIFORM_PROJECTION_MATRIX_PATH );
    TextureShader.mVmatrixUniform1 =  glGetUniformLocation( mTexShaderProgram->getProgram(), UNIFORM_VIEW_MATRIX_PATH );
    TextureShader.mMmatrixUniform1 =  glGetUniformLocation( mTexShaderProgram->getProgram(), UNIFORM_MATRIX_MODEL_PATH );
    // Add sampler uniform to the shader
    TextureShader.mTextureUniform1  =  glGetUniformLocation( mTexShaderProgram->getProgram(), UNIFORM_TEXTURE_PATH);
}

void RenderWindow::setupPhongShader()
{

    PhongShader.mUniformProjection = glGetUniformLocation(mPhongShaderProgram->getProgram(), PHONG_PROJECTION_PATH);
    PhongShader.mUniformModel = glGetUniformLocation(mPhongShaderProgram->getProgram(), PHONG_MODEL_PATH);
    PhongShader.mUniformView = glGetUniformLocation(mPhongShaderProgram->getProgram(), PHONG_VIEW_PATH);
    PhongShader.mTextureUniform = glGetUniformLocation(mTexShaderProgram->getProgram(), PHONG_TEXTURE_PATH);

    uniformDirectionalLight.uniformAmbientIntensity = glGetUniformLocation(mPhongShaderProgram->getProgram(), DIRECTIONALLIGHT_AMBIENT_INTENSITY);
    uniformDirectionalLight.uniformColor = glGetUniformLocation(mPhongShaderProgram->getProgram(), DIRECTIONALLIGHT_BASE_COLOR);
    uniformDirectionalLight.uniformDirection = glGetUniformLocation(mPhongShaderProgram->getProgram(), DIRECTIONALLIGHT_DIRECTION);
    uniformDirectionalLight.uniformDiffuseIntensity = glGetUniformLocation(mPhongShaderProgram->getProgram(), DIRECTIONALLIGHT_BASE_DIFFUSE_INTENSITY);
    PhongShader.mUniformSpecularIntensity = glGetUniformLocation(mPhongShaderProgram->getProgram(), PHONG_MATERIAL_SPECULAR_INTENSITY);
    PhongShader.mUniformShininess = glGetUniformLocation(mPhongShaderProgram->getProgram(), PHONG_MATERIAL_SHININESS);

    PhongShader.mUniformEyePosition = glGetUniformLocation(mPhongShaderProgram->getProgram(), PHONG_MATERIAL_EYE_POSITION);

    uniformPointLightCount = glGetUniformLocation(mPhongShaderProgram->getProgram(), POINTLIGHT_COUNT);

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

    if(event->key() == Qt::Key_Left)
    {
        rotation = -2;
    }

    if(event->key() == Qt::Key_Right)
    {
        rotation = 2;
    }

    if (event->key() == Qt::Key_Space)
    {
        bFirstPerson = !bFirstPerson;
    }

    if (event->key() == Qt::Key_L)
    {
        bCubeLight = !bCubeLight;
    }

//vvvvvvvvvvvvvvvvvvvvvvvvvvvvCattingvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
    if(event->key() == Qt::Key_1)
    {
        catShader = CatShader::PhongShader;
    }

    if(event->key() == Qt::Key_2)
    {
        catShader = CatShader::UnlitTextureShader;
    }

    if(event->key() == Qt::Key_3)
    {
        catShader = CatShader::PlainShader;
    }
//^^^^^^^^^^^^^^^^^^^^^^^^^^^^Catting^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
//    std::cout << "WorldPos: \n";
//    std::cout << "x: " << miaCollision->getPosition().x() << " y: " << mia->getPosition().y() << " z: " << mia->getPosition().z() << "\n";
}

// Player movement, with barycentric coordinates.
// Height (z value) is retrieved from the heightmap.

void RenderWindow::moveMiaX(float movespeed)
{
    glm::vec3 playerpos = glm::vec3(mia->getPosition().x(), mia->getPosition().y(), mia->getPosition().z());
    float height = heightMap->GetSurfaceHeight(playerpos) - miaCollision->getPosition().z();

    //movespeed = (movespeed * ));

    miaCollision->move(movespeed*(1/miaCollision->getRadius()), 0.0f, height*1/miaCollision->getRadius());


    miaCollision->GetWorldPosition() += QVector3D{movespeed, 0.0f, height};
    // QVector3D a = {movespeed, 0.0f, height};
    // miaCollision->SetWorldPosition(a);

    mia->move(movespeed, 0.0f, height);
    mia->GetWorldPosition() += QVector3D{movespeed, 0.0f, height};
    // QVector3D b = {movespeed, 0.0f, height}; // i mean we are doing the same thing twice lol
    // mia->SetWorldPosition(b);
}

void RenderWindow::moveMiaY(float movespeed)
{
    glm::vec3 playerpos = glm::vec3(mia->getPosition().x(), mia->getPosition().y(), mia->getPosition().z());
    float height = heightMap->GetSurfaceHeight(playerpos) - miaCollision->getPosition().z();


    miaCollision->move(0.0f, movespeed * (1/miaCollision->getRadius()), height*1/miaCollision->getRadius());
    miaCollision->GetWorldPosition() += QVector3D{0.0f, movespeed, height};
    //QVector3D y = {0.0f, movespeed, height};
    //miaCollision->SetWorldPosition(y);

    mia->move(0.0f,movespeed,height);
    mia->GetWorldPosition() += QVector3D{0.0f, movespeed, height};
    //mia->SetWorldPosition(y); // i think this set position is bugggedd
}

void RenderWindow::ToggleCollision()
{
    if (mCollision == false)
    {
        for (int i = 0; i < mItems.size(); i++)
            mItems[i]->bShouldRender = false;

        miaCollision->bShouldRender = false;
        doorCollision->bShouldRender = false;
    }
    else
    {
        for (int i = 0; i < mItems.size(); i++)
            mItems[i]->bShouldRender = true;

        miaCollision->bShouldRender = true;
        doorCollision->bShouldRender = true;
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





