#ifndef RENDERWINDOW_H
#define RENDERWINDOW_H

#include <QWindow>
#include <QOpenGLFunctions_4_1_Core>
#include <QTimer>
#include <QElapsedTimer>
#include <vector>
#include <list>
#include <unordered_map>

#include "CustomFiles/CommonValues.h"
#include "Light/light.h"
#include "Light/material.h"
#include <Light/directionallight.h>
#include <Light/pointlight.h>

#include "triangulation.h"
#include "visualobject.h"
#include "trianglesurface.h"
#include "interactiveobject.h"
#include "camera.h"
#include "cube.h"
#include "octahedronball.h"
#include "disc.h"
#include "npc.h"
#include "house.h"
#include "texture.h"
#include "mesh.h"
#include "heightmap.h"
#include "skybox.h"
#include "objmesh.h"
#include "plane.h"



class QOpenGLContext;
class Shader;
class MainWindow;

/// This inherits from QWindow to get access to the Qt functionality and
// OpenGL surface.
// We also inherit from QOpenGLFunctions, to get access to the OpenGL functions
// This is the same as using "glad" and "glw" from general OpenGL tutorials
class RenderWindow : public QWindow, protected QOpenGLFunctions_4_1_Core
{
    Q_OBJECT
public:
    RenderWindow(const QSurfaceFormat &format, MainWindow *mainWindow);
    ~RenderWindow() override;

    QOpenGLContext *context() { return mContext; }

    void exposeEvent(QExposeEvent *) override;  //gets called when app is shown and resized

    bool mRotate{true};
    bool mPathOne{false};
    bool mCollision{false};
    bool mWireframe{false};

private slots:
    void render();          //the actual render - function

private:
    void init();            //initialize things we need before rendering
    std::vector<VisualObject*> mObjects;
    std::vector<VisualObject*> mItems; // kollisjonsvolum
    InteractiveObject* mia;
    VisualObject* miaCollision;
    VisualObject* cat;
    Plane* door;

    QVector3D followCamera;
    QVector3D playerPos;

    float rotation;

    int playerPoints{0};

    Triangulation* triangulation;

    Camera mCamera{};
    int catting=0;
    // NPCs
    VisualObject* BOT;
    VisualObject* BOT2;
    VisualObject* Path;
    VisualObject* Path2;
    void CollisionHandling();

    House* house;
    std::unordered_map<std::string, VisualObject*> mMap;    // alternativ container

    QOpenGLContext *mContext{nullptr};  //Our OpenGL context
    bool mInitialized{false};

    void setupPlainShader();
    GLint  mPmatrixUniform0{};
    GLint  mVmatrixUniform0{};
    GLint  mMmatrixUniform0{};

    void setupTextureShader();
    GLint  mPmatrixUniform1{};
    GLint  mVmatrixUniform1{};
    GLint  mMmatrixUniform1{};
    GLint  mTextureUniform1{};

    void  setupPhongShader();
    void  SetDirectionalLight(DirectionalLight* dLight);
    void  SetPointLights();
    std::string GetUniformLocationString(const std::string& uniformName, int index);

    int   pointLightCount, PointLightCount = 0;

    GLint mUniformProjection{};
    GLint mUniformModel{};
    GLint mUniformView{};
    GLint mTextureUniform{};
    GLint mUniformEyePosition{};
    GLuint mUniformSpecularIntensity{}, mUniformShininess{};

    struct {
        GLuint uniformColor{};
        GLuint uniformAmbientIntensity{};
        GLuint uniformDiffuseIntensity{};

        GLuint uniformDirection{};
    } uniformDirectionalLight;

    GLuint uniformPointLightCount;

    struct {
        GLuint uniformColor{};
        GLuint uniformAmbientIntensity{};
        GLuint uniformDiffuseIntensity{};

        GLuint uniformPosition{};
        GLuint uniformConstant{};
        GLuint uniformLinear{};
        GLuint uniformExponent{};
    } uniformPointLight[MAX_POINT_LIGHTS];

    DirectionalLight *mainLight;
    //PointLight *pointLights[MAX_POINT_LIGHTS];

    std::vector<PointLight*> mPointLights;

    Texture *brickTexture;
    Texture *dirtTexture;
    Texture *plainTexture;
    Texture *woodTexture;
    Texture *crateTexture;
    Texture *catTexture;

    HeightMap *heightMap;

    Skybox *skybox;

    std::vector<ObjMesh*> mModels;

    Material *shinyMaterial;
    Material *dullMaterial;

    Shader *mPlainShaderProgram{nullptr};    //holds pointer the GLSL shader program
    Shader *mTexShaderProgram{nullptr};
    Shader *mPhongShaderProgram{nullptr};

    GLuint mVAO;                        //OpenGL reference to our VAO
    GLuint mVBO;                        //OpenGL reference to our VBO

    QMatrix4x4 *mMVPmatrix{nullptr};       //The matrix with the transform for the object we draw
    QMatrix4x4 *mPmatrix{nullptr};
    QMatrix4x4 *mVmatrix{nullptr};

    QTimer *mRenderTimer{nullptr};           //timer that drives the gameloop
    QElapsedTimer mTimeStart;               //time variable that reads the calculated FPS

    MainWindow *mMainWindow{nullptr};        //points back to MainWindow to be able to put info in StatusBar

    class QOpenGLDebugLogger *mOpenGLDebugLogger{nullptr};  //helper class to get some clean debug info from OpenGL
    class Logger *mLogger{nullptr};         //logger - Output Log in the application

    ///Helper function that uses QOpenGLDebugLogger or plain glGetError()
    void checkForGLerrors();

    void calculateFramerate();          //as name says

    ///Starts QOpenGLDebugLogger if possible
    void startOpenGLDebugger();

protected:
    //The QWindow that we inherit from have these functions to capture
    // - mouse and keyboard.
    // Uncomment to use (you also have to make the definitions of
    // these functions in the cpp-file to use them of course!)
    //
    //    void mousePressEvent(QMouseEvent *event) override{}
    //    void mouseMoveEvent(QMouseEvent *event) override{}
    void keyPressEvent(QKeyEvent *event) override;              //the only one we use now
    //    void keyReleaseEvent(QKeyEvent *event) override{}
    //    void wheelEvent(QWheelEvent *event) override{}

private:
    void moveMiaX(float movespeed);
    void moveMiaY(float movespeed);

    glm::vec3 playerLight{0,0,0};

    bool bSceneOne = true;
    bool bShouldMove = true;

    void ToggleCollision();
    void TogglePath();
    void ToggleWireframe();
};

#endif // RENDERWINDOW_H
