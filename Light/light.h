#ifndef LIGHT_H
#define LIGHT_H

#include <QOpenGLFunctions_4_1_Core>
#include <glm.hpp>

class Light : public QOpenGLFunctions_4_1_Core
{
public:
    Light();
    Light(GLfloat red, GLfloat green, GLfloat blue, GLfloat aIntensity);
    ~Light() { };

    void UseLight(GLfloat ambientIntensityLocation, GLfloat ambientColorLocation);

private:
    glm::vec3 color;
    GLfloat ambientIntensity;
};

#endif // LIGHT_H
