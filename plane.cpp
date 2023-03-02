#include "plane.h"

Plane::Plane()
{
   mVertices.push_back(Vertex{0, 0,   0,          1,1,0});
   mVertices.push_back(Vertex{1, 0,   0,        1,1,0});
   mVertices.push_back(Vertex{1, 0,   1,          1,1,0});

   mVertices.push_back(Vertex{1, 0,   1,          0,1,0});
   mVertices.push_back(Vertex{0, 0,   1,       0,1,0});
   mVertices.push_back(Vertex{0, 0,   0,          0,1,0});


   mMatrix.setToIdentity();
}

Plane::~Plane()
{

}

void Plane::init(GLint matrixUniform)
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
    mMatrix.translate(-3.f, -1.0f, 0.0f);
    mMatrix.scale(2,2,2);
}

void Plane::draw()
{
    glBindVertexArray( mVAO );
    glUniformMatrix4fv( mMatrixUniform, 1, GL_FALSE, mMatrix.constData());
    glDrawArrays(GL_TRIANGLES, 0, mVertices.size());
    //mMatrix.rotate(1.0f, -1.0f, 2.0f, 0.5f);
}



void Plane::SetRotation(float angle, float x, float y, float z)
{
    mMatrix.rotate(angle, x, y, z);
}


void Plane::SetLocation(float x, float y, float z)
{
    mMatrix.translate(x,y,z);

}



