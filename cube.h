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
    ~Cube();
    void init(GLint matrixUniform) override;
    void draw() override;
};

#endif // CUBE_H
