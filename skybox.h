#ifndef SKYBOX_H
#define SKYBOX_H

#include <string>
#include <vector>

#include <QOpenGLFunctions_4_1_Core>

#include <glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "CustomFiles/CommonValues.h"
#include "shader.h""

#include "qmatrix4x4.h"

class Skybox : public QOpenGLFunctions_4_1_Core
{
public:
    Skybox();
    Skybox(std::vector<std::string> faceLocations);
    void CreateMesh(GLfloat *vertices, unsigned int *indices, unsigned int numOfVertices, unsigned int numOfIndices);
    void DrawSkybox(QMatrix4x4 viewMatrix, QMatrix4x4 projectionMatrix);
    void ClearMesh();

    ~Skybox() { ClearMesh(); }
private:
    GLuint VAO, VBO, IBO;
    GLsizei indexCount;

    Shader *skyShader{nullptr};
    GLuint textureID;
    GLuint uniformProjection, uniformView;
};

#endif // SKYBOX_H
