#include "tetrahedron.h"

Tetrahedron::Tetrahedron()
{
    constructTetrahedron();
    mMatrix.setToIdentity();
}

Tetrahedron::Tetrahedron(float x, float y, float s)
{
    constructTetrahedron();
    translatePoint(x,y,s);
}

void Tetrahedron::translatePoint(float x, float y, float s)
{
    mMatrix.translate(x,y,0);
    mMatrix.scale(s);
}


Tetrahedron::~Tetrahedron()
{

}

void Tetrahedron::constructTetrahedron()
{
    mVertices.push_back(Vertex{-0.3, -0.3,   0,          1,1,0});
    mVertices.push_back(Vertex{0.3,  -0.3,   0.3,        1,1,0});
    mVertices.push_back(Vertex{0,    0.3,    0,          1,1,0});

    mVertices.push_back(Vertex{-0.3, -0.3,   0,          0,1,0});
    mVertices.push_back(Vertex{0.3,  -0.3,   -0.3,       0,1,0});
    mVertices.push_back(Vertex{0,    0.3,    0,          0,1,0});

    mVertices.push_back(Vertex{0.3,  -0.3,   0.3,        0,0,1});
    mVertices.push_back(Vertex{0.3,  -0.3,   -0.3,       0,0,1});
    mVertices.push_back(Vertex{0,    0.3,    0,          0,0,1});

    mVertices.push_back(Vertex{0.3,  -0.3,   0.3,        1,0,0});
    mVertices.push_back(Vertex{-0.3, -0.3,   0.0,        1,0,0});
    mVertices.push_back(Vertex{0.3,  -0.3,   -0.3,       1,0,0});
}

void Tetrahedron::init(GLint matrixUniform)
{
    mMatrixUniform = matrixUniform;
    initializeOpenGLFunctions();

    glGenVertexArrays(1, &mVAO);
    glBindVertexArray(mVAO);

    glGenBuffers(1, &mVBO);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);

    glBufferData( GL_ARRAY_BUFFER, mVertices.size()*sizeof(Vertex), mVertices.data(), GL_STATIC_DRAW );

    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glVertexAttribPointer(
                0,
                3,
                GL_FLOAT,
                GL_FALSE,
                sizeof(Vertex),
                reinterpret_cast<GLvoid*>(0));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,  sizeof(Vertex),  reinterpret_cast<GLvoid*>(3 * sizeof(GLfloat)) );
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);
    mMatrix.translate(-0.5f, 0.0f, 0.2f);
}

void Tetrahedron::draw()
{
    glBindVertexArray( mVAO );
    glUniformMatrix4fv( mMatrixUniform, 1, GL_FALSE, mMatrix.constData());
    glDrawArrays(GL_TRIANGLES, 0, mVertices.size());

    mMatrix.rotate(2.0f, -1.0f, 2.0f, 0.5f);
}

