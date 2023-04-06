#ifndef LIGHT_H
#define LIGHT_H

#include <QOpenGLFunctions_4_1_Core>
#include <glm.hpp>

class Light : public QOpenGLFunctions_4_1_Core
{
public:
    Light();
    Light(GLfloat red, GLfloat green, GLfloat blue,
          GLfloat aIntensity,
          GLfloat xDir, GLfloat yDir, GLfloat zDir,
          GLfloat dIntensity);
    ~Light() { };

    void UseLight(GLuint ambientIntensityLocation, GLuint ambientColorLocation,
                  GLuint diffuseIntensityLocation, GLuint directionLocation);

private:
    glm::vec3 color;
    GLfloat ambientIntensity;

    glm::vec3 direction;
    GLfloat diffuseIntensity;
};

#endif // LIGHT_H
