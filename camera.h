#ifndef CAMERA_H
#define CAMERA_H

#include <QOpenGLFunctions_4_1_Core>
#include <QVector3D>
#include <QMatrix4x4>
#include <glm.hpp>

class Camera : public QOpenGLFunctions_4_1_Core
{
private:
   float A[4][4] = {};
   GLint  mPmatrixUniform;
   GLint  mVmatrixUniform;
   QVector3D mEye;
   float& operator () (int i, int j) {return A[i][j]; }
public:
   Camera();
   ~Camera();
   void init(GLint pMatrixUniform, GLint vMatrixUniform);
   void perspective(int fovy, double aspect, double nearplane, double farplane);
   void lookAt(const QVector3D& eye, const QVector3D& at, const QVector3D& up);
   void rotateAroundTarget(QVector3D target, float deltaX, float deltaY);
   void rotateAroundTargetFP(QVector3D target, float deltaX, float deltaY);
   void update();


   QMatrix4x4 mPmatrix{};
   QMatrix4x4 mVmatrix{};

   glm::vec3 getCameraPosition();
   //void translate(float dx, float dy, float dz);
private:
    // Camera position and orientation
    glm::vec3 m_position;
    glm::vec3 m_target;
    glm::vec3 m_up;
    float m_yaw;
    float m_pitch;
    // Distance between camera and target
    float m_distance = 4.0f;
};

#endif // CAMERA_H
