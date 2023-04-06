#ifndef MATERIAL_H
#define MATERIAL_H

#include <QOpenGLFunctions_4_1_Core>
#include <QOpenGLFunctions>

class Material : protected QOpenGLFunctions_4_1_Core
{
public:
    Material();
    Material(GLfloat sIntensity, GLfloat shine);
    ~Material() {};

    void UseMaterial(GLuint specularIntensityLocation, GLuint shininessLocation);

private:
    GLfloat specularIntensity;
    GLfloat shininess;
};

#endif // MATERIAL_H
