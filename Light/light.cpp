#include "light.h"

Light::Light()
{
    initializeOpenGLFunctions();

    color = glm::vec3(1.0f, 1.0f, 1.0f);
    ambientIntensity = 1.0f;
}

Light::Light(GLfloat red, GLfloat green, GLfloat blue, GLfloat aIntensity)
{
    initializeOpenGLFunctions();

    color = glm::vec3(red, green, blue);
    ambientIntensity = aIntensity;
}


void Light::UseLight(GLfloat ambientIntensityLocation, GLfloat ambientColorLocation)
{
    glUniform3f(ambientColorLocation, color.x, color.y, color.z);
    glUniform1f(ambientIntensityLocation, ambientIntensity);
}
