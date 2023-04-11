#ifndef OBJMESH_H
#define OBJMESH_H

#include "visualobject.h"

#include <glm/glm.hpp>

class ObjMesh : public VisualObject
{
public:
    ObjMesh() {};
    ObjMesh(std::string filename);
    ~ObjMesh(){};

    void init(GLint matrixUniform) override;
    void draw() override;
private:
    void readFile(std::string filename);
};

#endif // OBJMESH_H
