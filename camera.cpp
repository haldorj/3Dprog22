#include "camera.h"
#include <iostream>

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
    mEye = eye;
}

void Camera::rotateAroundTarget(QVector3D target, float deltaX, float deltaY)
{
     // Calculate new camera position
     m_yaw += deltaX;
     m_pitch += deltaY;
     //m_pitch = glm::clamp(m_pitch, -89.0f, 89.0f); // prevent flipping over
     glm::vec3 cameraPosition;
     cameraPosition.x = target.x() + m_distance * cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
     cameraPosition.y = target.y() + m_distance * sin(glm::radians(m_pitch));
     cameraPosition.z = target.z() + m_distance * sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
     //m_position = cameraPosition;
     QVector3D cameraPos{cameraPosition.x, cameraPosition.y, cameraPosition.z + 1.5f};
     mVmatrix.lookAt(cameraPos,
                     QVector3D{target.x(), target.y(), target.z() + 1.0f},
                     QVector3D{0,0,1});
     mEye = cameraPos;
 }


void Camera::update()
{
    initializeOpenGLFunctions();
    glUniformMatrix4fv(mPmatrixUniform, 1, GL_FALSE, mPmatrix.constData());
    glUniformMatrix4fv(mVmatrixUniform, 1, GL_FALSE, mVmatrix.constData());
}

glm::vec3 Camera::getCameraPosition()
{
    glm::vec3 position = {mEye.x(),
                          mEye.y(),
                          mEye.z()};
    return position;
}

//void Camera::translate(float dx, float dy, float dz){}
