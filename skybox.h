#ifndef SKYBOX_H
#define SKYBOX_H

#include <string>
#include <vector>

#include <QOpenGLFunctions_4_1_Core>

#include "CustomFiles/CommonValues.h" // has stb_image included
#include "shader.h"

#include "qmatrix4x4.h"

class Skybox : public QOpenGLFunctions_4_1_Core
{
public:
    Skybox();
    ~Skybox();
    Skybox(std::vector<std::string> faceLocations);
    void CreateMesh(GLfloat *vertices, unsigned int *indices, unsigned int numOfVertices, unsigned int numOfIndices);

    // We need the view and projection matrix of our camera when drawing the skybox
    void DrawSkybox(QMatrix4x4 viewMatrix, QMatrix4x4 projectionMatrix);
    void ClearMesh();


private:
    GLuint VAO, VBO, IBO;
    GLsizei indexCount;

    Shader *skyShader{nullptr};
    GLuint textureID;
    GLuint uniformProjection, uniformView;
};

#endif // SKYBOX_H
