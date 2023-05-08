#ifndef CUBE_H
#define CUBE_H

#include <QOpenGLFunctions_4_1_Core>
#include <QMatrix4x4>
#include <vector>
#include "vertex.h"
#include "visualobject.h"

class Cube : public VisualObject
{
public:
    Cube();
    Cube(float size,float x, float y, float z, float r, float g, float b);
    ~Cube();

    void init(GLint matrixUniform) override;
    void draw() override;

    bool bShouldRender = true;
};

#endif // CUBE_H
