#ifndef TETRAHEDRON_H
#define TETRAHEDRON_H

#include <QOpenGLFunctions_4_1_Core>
#include <QMatrix4x4>
#include <vector>
#include "vertex.h"
#include "visualobject.h"


class Tetrahedron : public VisualObject
{
public:
    Tetrahedron();
    Tetrahedron(float x, float y, float z, float s);
    ~Tetrahedron() override;
    void constructTetrahedron();
    void init(GLint matrixUniform) override;
    void draw() override;

    void translatePoint(float x, float y, float z, float s);
};

#endif // TETRAHEDRON_H
