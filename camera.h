#ifndef CAMERA_H
#define CAMERA_H

#include <QOpenGLFunctions_4_1_Core>
#include <QVector3D>
#include <QMatrix4x4>

class Camera : public QOpenGLFunctions_4_1_Core
{
private:
   float A[4][4] = {};
   GLint  mPmatrixUniform;
   GLint  mVmatrixUniform;
   QVector3D mEye;
   QMatrix4x4 mPmatrix{};
   QMatrix4x4 mVmatrix{};
   float& operator () (int i, int j) {return A[i][j]; }
public:
   Camera();
   ~Camera();
   void init(GLint pMatrixUniform, GLint vMatrixUniform);
   void perspective(int fovy, double aspect, double nearplane, double farplane);
   void lookAt(const QVector3D& eye, const QVector3D& at, const QVector3D& up);
   void update();
   //void translate(float dx, float dy, float dz);
};

#endif // CAMERA_H
