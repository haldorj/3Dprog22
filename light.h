#ifndef LIGHT_H
#define LIGHT_H

#include <QOpenGLFunctions_4_1_Core>

#include <glm.hpp>

class Light : protected QOpenGLFunctions_4_1_Core
{
public:
    Light();
    Light(	GLfloat red, GLfloat green, GLfloat blue,
            GLfloat aIntensity, GLfloat dIntensity	);
    ~Light() { };
    void calcAverageNormals(unsigned int* indices, unsigned int indexCount, GLfloat* vertices, unsigned int vertexCount,
        unsigned int vLength, unsigned int normalOffset);
// protected: sub-classes can access
protected:
    glm::vec3 color;
    GLfloat ambientIntensity;
    GLfloat diffuseIntensity;
};

#endif // LIGHT_H
