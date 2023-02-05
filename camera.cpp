#include "camera.h"

Camera::Camera() : mEye{0.0f, 0.0f, 0.0f}
{
    //mPmatrix.setToIdentity();
    //mVmatrix.setToIdentity();
}

Camera::~Camera(){ }

void Camera::init(GLint pMatrixUniform, GLint vMatrixUniform)
{
    mPmatrix.setToIdentity();
    mVmatrix.setToIdentity();
    mPmatrixUniform = pMatrixUniform;
    mVmatrixUniform = vMatrixUniform;
}

// Perspective Matrix
void Camera::perspective(int fovy, double aspect, double nearplane, double farplane)
{
    mPmatrix.perspective(fovy, aspect, nearplane, farplane);
}

// View Matrix
void Camera::lookAt(const QVector3D &eye, const QVector3D &at, const QVector3D &up)
{
    mVmatrix.lookAt(eye, at, up);
}


void Camera::update()
{
    initializeOpenGLFunctions();
    glUniformMatrix4fv(mPmatrixUniform, 1, GL_FALSE, mPmatrix.constData());
    glUniformMatrix4fv(mVmatrixUniform, 1, GL_FALSE, mVmatrix.constData());
}

//void Camera::translate(float dx, float dy, float dz){}
