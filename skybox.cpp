#include "skybox.h"

Skybox::Skybox()
{
    initializeOpenGLFunctions();
}

Skybox::~Skybox()
{
    ClearMesh();
}

Skybox::Skybox(std::vector<std::string> faceLocations)
{
    initializeOpenGLFunctions();

    // Shader setup
    skyShader = new Shader( "../3Dprog22/skybox.vert",
                            "../3Dprog22/skybox.frag");

    uniformProjection = glGetUniformLocation( skyShader->getProgram(), "projection" );
    uniformView = glGetUniformLocation( skyShader->getProgram(), "view" );

    // Texture setup (cube mapping)
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, bitDepth;

    // Load the textures for each of the faces of the cube
    for (size_t i = 0; i < 6; i++)
    {
        unsigned char* texData = stbi_load(faceLocations[i].c_str(), &width, &height, &bitDepth, 0);
        if (!texData)
        {
            printf("failed to find %s\n", faceLocations[i].c_str());
            return;
        }
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, texData);
        stbi_image_free(texData);
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    // Mesh Setup
    unsigned int skyboxIndices[] = {
        // front
        0, 1, 2,
        2, 1, 3,
        // right
        2, 3, 5,
        5, 3, 7,
        // back
        5, 7, 4,
        4, 7, 6,
        // left
        4, 6, 0,
        0, 6, 1,
        // top
        4, 0, 5,
        5, 0, 2,
        // bottom
        1, 6, 3,
        3, 6, 7
    };

    float skyboxVertices[] = {
        -1.0f, 1.0f, -1.0f,		0.0f, 0.0f, 0.0f,     0.0f, 0.0f,
        -1.0f, -1.0f, -1.0f,	0.0f, 0.0f, 0.0f,     0.0f, 0.0f,
        1.0f, 1.0f, -1.0f,		0.0f, 0.0f, 0.0f,     0.0f, 0.0f,
        1.0f, -1.0f, -1.0f,		0.0f, 0.0f, 0.0f,     0.0f, 0.0f,

        -1.0f, 1.0f, 1.0f,		0.0f, 0.0f, 0.0f,     0.0f, 0.0f,
        1.0f, 1.0f, 1.0f,		0.0f, 0.0f, 0.0f,     0.0f, 0.0f,
        -1.0f, -1.0f, 1.0f,		0.0f, 0.0f, 0.0f,     0.0f, 0.0f,
        1.0f, -1.0f, 1.0f,		0.0f, 0.0f, 0.0f,     0.0f, 0.0f,
    };

    CreateMesh(skyboxVertices, skyboxIndices, 64, 36);
}

void Skybox::CreateMesh(GLfloat *vertices, unsigned int *indices, unsigned int numOfVertices, unsigned int numOfIndices)
{
    // init function
    indexCount = numOfIndices;

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &IBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * numOfIndices, indices, GL_STATIC_DRAW);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * numOfVertices, vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertices[0]) * 8, 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(vertices[0]) * 8, (void*)(sizeof(vertices[0]) * 3));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(vertices[0]) * 8, (void*)(sizeof(vertices[0]) * 5));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glBindVertexArray(0);
}

void Skybox::DrawSkybox(QMatrix4x4 viewMatrix, QMatrix4x4 projectionMatrix)
{
    viewMatrix = QMatrix4x4(viewMatrix.toGenericMatrix<3, 3>());

    glDepthMask(GL_FALSE);

    glUseProgram(skyShader->getProgram());

    glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, projectionMatrix.constData());
    glUniformMatrix4fv(uniformView, 1, GL_FALSE, viewMatrix.constData());

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glDepthMask(GL_TRUE);
}

void Skybox::ClearMesh()
{
    if (IBO != 0)
    {
        glDeleteBuffers(1, &IBO);
        IBO = 0;
    }

    if (VBO != 0)
    {
        glDeleteBuffers(1, &VBO);
        VBO = 0;
    }

    if (VAO != 0)
    {
        glDeleteVertexArrays(1, &VAO);
        VAO = 0;
    }

    indexCount = 0;
}
