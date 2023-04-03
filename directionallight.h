#ifndef DIRECTIONALLIGHT_H
#define DIRECTIONALLIGHT_H

#include "Light.h"

class DirectionalLight :
    public Light
{
public:
    DirectionalLight();
    DirectionalLight(GLfloat red, GLfloat green, GLfloat blue,	// Defined in super class
                     GLfloat aIntensity, GLfloat dIntensity,	// Defined in super class
                     GLfloat xDir, GLfloat yDir, GLfloat zDir); // Specific for sub class

    ~DirectionalLight() { };

    void UseLight(GLuint ambientIntensityLocation, GLuint ambientColorLocation,
        GLuint diffuseIntensityLocation, GLuint directionLocation);
private:
    glm::vec3 direction;
};

#endif // DIRECTIONALLIGHT_H
