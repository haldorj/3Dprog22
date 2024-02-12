#ifndef POINTLIGHT_H
#define POINTLIGHT_H

#include "light.h"

class PointLight :
    public Light
{
public:
    PointLight();
    PointLight(GLfloat red, GLfloat green, GLfloat blue,
        GLfloat aIntensity, GLfloat dIntensity,
        GLfloat xPos, GLfloat yPos, GLfloat zPos,
        GLfloat con, GLfloat lin, GLfloat exp);
    ~PointLight(){ };

    void setLightColor(GLfloat red, GLfloat green, GLfloat blue,
                  GLfloat aIntensity, GLfloat dIntensity);

    void UseLight(GLuint ambientIntensityLocation, GLuint ambientColorLocation,
        GLuint diffuseIntensityLocation, GLuint positionLocation,
        GLuint constantLocation, GLuint linearLocation, GLuint exponentLocation);

protected:
    glm::vec3 position;

    GLfloat constant, linear, exponent;

    // x dist
    // a exponent, b linear, c constant
    //ax^2 + bx + c
public:
    void setPos(float x, float y, float z) {position = glm::vec3(x,y,z);}
};


#endif // POINTLIGHT_H
