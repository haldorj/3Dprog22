#include "xyz.h"

XYZ::XYZ()
{
    // X, red
   mVertices.push_back(Vertex{0,0,0,1,0,0});
   mVertices.push_back(Vertex{1,0,0,1,0,0});
   // Y, green
   mVertices.push_back(Vertex{0,0,0,0,1,0});
   mVertices.push_back(Vertex{0,1,0,0,1,0});
   // Z, blue
   mVertices.push_back(Vertex{0,0,0,0,0,1});
   mVertices.push_back(Vertex{0,0,1,0,0,1});

   mMatrix.setToIdentity();
}

XYZ::~XYZ()
{

}

void XYZ::init(GLint matrixUniform)
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
}

void XYZ::draw()
{
   glBindVertexArray( mVAO );
   glUniformMatrix4fv( mMatrixUniform, 1, GL_FALSE, mMatrix.constData());
   glDrawArrays(GL_LINES, 0, mVertices.size());
}
