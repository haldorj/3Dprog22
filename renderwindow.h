#ifndef RENDERWINDOW_H
#define RENDERWINDOW_H

#include <QWindow>
#include <QOpenGLFunctions_4_1_Core>
#include <QTimer>
#include <QElapsedTimer>
#include <vector>
#include <list>
#include <unordered_map>

#include <Light/light.h>

#include "triangulation.h"
#include "visualobject.h"
#include "trianglesurface.h"
#include "interactiveobject.h"
#include "camera.h"
#include "cube.h"
#include "octahedronball.h"
#include "disc.h"
#include "npc.h"
#include "texture.h"
#include "mesh.h"
#include "heightmap.h"


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

private slots:
    void render();          //the actual render - function

private:
    void init();            //initialize things we need before rendering
    std::vector<VisualObject*> mObjects;
    std::vector<VisualObject*> mItems; // kollisjonsvolum
    InteractiveObject* mia;
    VisualObject* miaCollision;

    Triangulation* triangulation;
    std::vector<glm::vec2> mTriangles;
    glm::vec3 barycentricCoordinates(const glm::vec2& p1, const glm::vec2& p2, const glm::vec2& p3, const glm::vec2& pt);
    float GetSurfaceHeight();

    VisualObject* heightmap;

    Camera mCamera{};

    // NPCs
    VisualObject* BOT;
    VisualObject* BOT2;
    VisualObject* Path;
    VisualObject* Path2;
    void CollisionHandling();

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

   // void calcAverageNormals(unsigned int* indices, unsigned int indexCount, GLfloat* vertices, unsigned int vertexCount,
     //   unsigned int vLength, unsigned int normalOffset);

    void  setupPhongShader();
    GLint mUniformProjection{};
    GLint mUniformModel{};
    GLint mUniformView{};
    GLint mTextureUniform{};

    GLuint mUniformAmbientIntensity{} , mUniformAmbientColor{}, mUniformDirection{}, mUniformDiffuseIntensity{};

    Light *mainLight;

    Texture *brickTexture;
    Texture *dirtTexture;
    Texture *plainTexture;

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

    bool bSceneOne = true;
    bool bShouldMove = true;

    void ToggleCollision();
    void TogglePath();

};

#endif // RENDERWINDOW_H
