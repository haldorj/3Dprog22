#include "interactiveobject.h"

InteractiveObject::InteractiveObject() : mx{0.0f}, my{0.0f}, mz{0.0f}
{
    initCubeGeometry();
    calcAverageNormals();
    mMatrix.translate(0, 0, radius);
}

InteractiveObject::~InteractiveObject()
{

}

void InteractiveObject::init(GLint matrixUniform)
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

void InteractiveObject::draw()
{
    glBindVertexArray( mVAO );
    // Bindbuffer, indexed draws
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIBO);
    glUniformMatrix4fv( mMatrixUniform, 1, GL_FALSE, mMatrix.constData());
    // DrawElements, indexed draws
    glDrawElements(GL_TRIANGLES, mIndices.size(), GL_UNSIGNED_INT, nullptr);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void InteractiveObject::move(float x, float y, float z)
{
    mx += x;
    my += y;
    mz += z;
    mMatrix.translate(mx, my, mz);
    mx =0;
    my =0;
    mz =0;
}

void InteractiveObject::initCubeGeometry()
{
    float size = 0.3;
    radius = size;

    // Define the vertices of the cube
    // Front
    mVertices.push_back(Vertex{-size, -size,  size,    0.0, 0.0, 0.0,     0.0,  0.0}); // Bottom-left
    mVertices.push_back(Vertex{ size, -size,  size,    0.0, 0.0, 0.0,     1.0,  0.0}); // Bottom-right
    mVertices.push_back(Vertex{ size,  size,  size,    0.0, 0.0, 0.0,     1.0,  1.0}); // Top-right
    mVertices.push_back(Vertex{-size,  size,  size,    0.0, 0.0, 0.0,     0.0,  1.0}); // Top-left

    // Back
    mVertices.push_back(Vertex{-size, -size, -size,    0.0, 0.0, 0.0,      1.0,  0.0}); // Bottom-left
    mVertices.push_back(Vertex{ size, -size, -size,    0.0, 0.0, 0.0,      0.0,  0.0}); // Bottom-right
    mVertices.push_back(Vertex{ size,  size, -size,    0.0, 0.0, 0.0,      0.0,  1.0}); // Top-right
    mVertices.push_back(Vertex{-size,  size, -size,    0.0, 0.0, 0.0,      1.0,  1.0});  // Top-left

    // Define the index array
    mIndices = {
        // Front
        0, 1, 2,
        2, 3, 0,
        // Top
        3, 2, 6,
        6, 7, 3,
        // Back
        7, 6, 5,
        5, 4, 7,
        // Bottom
        4, 5, 1,
        1, 0, 4,
        // Left
        4, 0, 3,
        3, 7, 4,
        // Right
        1, 5, 6,
        6, 2, 1
    };
}
