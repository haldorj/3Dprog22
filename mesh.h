#ifndef MESH_H
#define MESH_H

#include <QOpenGLFunctions_4_1_Core>

class Mesh : protected QOpenGLFunctions_4_1_Core
{
public:
    Mesh();
    ~Mesh();
    void CreateMesh(GLfloat *vertices, unsigned int* indices, unsigned int numOfVertices, unsigned int numOfIndices);
    void RenderMesh();
    void ClearMesh();

private:
    GLuint VAO, VBO, IBO;
    GLsizei indexCount;
};

#endif // MESH_H
