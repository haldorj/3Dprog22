#include "house.h"

House::House()
{
    initCubeGeometry();
}

void House::init(GLint matrixUniform)
{
    mMatrixUniform = matrixUniform;
    initializeOpenGLFunctions();

    glGenVertexArrays(1, &mVAO);
    glBindVertexArray(mVAO);

    glGenBuffers(1, &mVBO);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);

    // Setup indexed draws
    glGenBuffers(1, &mIBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mIndices.size() * sizeof(GLuint), mIndices.data(), GL_STATIC_DRAW);

    glBufferData( GL_ARRAY_BUFFER, mVertices.size()*sizeof(Vertex), mVertices.data(), GL_STATIC_DRAW );
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(0));
    glEnableVertexAttribArray(0);
    // color/normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,  sizeof(Vertex),  (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    // texture coord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),  (GLvoid*)(6 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void House::draw()
{
    glBindVertexArray( mVAO );
    // Bindbuffer, indexed draws
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIBO);
    glUniformMatrix4fv( mMatrixUniform, 1, GL_FALSE, mMatrix.constData());
    // DrawElements, indexed draws
    glDrawElements(GL_TRIANGLES, mIndices.size(), GL_UNSIGNED_INT, nullptr);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void House::initCubeGeometry()
{
    float size = 2;
    radius = size;

    // Define the vertices of the cube

    // Front face
    mVertices.push_back(Vertex{-size, -size,  size, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f}); // Bottom left
    mVertices.push_back(Vertex{ size, -size,  size, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f}); // Bottom right
    mVertices.push_back(Vertex{ size,  size,  size, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f}); // Top right
    mVertices.push_back(Vertex{-size,  size,  size, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f}); // Top left

    // Back face
    mVertices.push_back(Vertex{ size, -size, -size, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f}); // Bottom left
    mVertices.push_back(Vertex{-size, -size, -size, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f}); // Bottom right
    mVertices.push_back(Vertex{-size,  size, -size, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f}); // Top right
    mVertices.push_back(Vertex{ size,  size, -size, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f}); // Top left

    // Top face
    mVertices.push_back(Vertex{-size,  size,  size, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f}); // Bottom left
    mVertices.push_back(Vertex{ size,  size,  size, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f}); // Bottom right
    mVertices.push_back(Vertex{ size,  size, -size, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f}); // Top right
    mVertices.push_back(Vertex{-size,  size, -size, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f}); // Top left

    // Bottom face
    mVertices.push_back(Vertex{ size, -size,  size, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f}); // Bottom left
    mVertices.push_back(Vertex{-size, -size,  size, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f}); // Bottom right
    mVertices.push_back(Vertex{-size, -size, -size, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f}); // Top right
    mVertices.push_back(Vertex{ size, -size, -size, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f}); // Top left

    // Right face
    mVertices.push_back(Vertex{ size, -size,  size, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f}); // Bottom left
    mVertices.push_back(Vertex{ size, -size, -size, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f}); // Bottom right
    mVertices.push_back(Vertex{ size,  size, -size, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f}); // Top right
    mVertices.push_back(Vertex{ size,  size,  size, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f}); // Top left

    // Left face
    mVertices.push_back(Vertex{-size, -size, -size, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f}); // Bottom left
    mVertices.push_back(Vertex{-size, -size,  size, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f}); // Bottom right
    mVertices.push_back(Vertex{-size,  size,  size, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f}); // Top right
    mVertices.push_back(Vertex{-size,  size, -size, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f}); // Top left

    // Define the index array
    mIndices = {
        0, 1, 2, // Front face
        2, 3, 0,
        4, 5, 6, // Back face
        6, 7, 4,
        8, 9, 10, // Top face
        10, 11, 8,
        12, 13, 14, // Bottom face
        14, 15, 12,
        16, 17, 18, // Right face
        18, 19, 16,
        20, 21, 22, // Left face
        22, 23, 20
    };

}

void House::move(float x, float y, float z)
{
    mx += x;
    my += y;
    mz += z;
    mMatrix.translate(mx, my, mz);
    mWorldPosition+=QVector3D(mx,my,mz);
    mx =0;
    my =0;
    mz =0;

}
