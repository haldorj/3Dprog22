#include "plane.h"

Plane::Plane()
{
   mVertices.push_back(Vertex{0, 0,   0,    0.65, 0.43, 0.3});
   mVertices.push_back(Vertex{1, 0,   0,    0.65, 0.43, 0.3});
   mVertices.push_back(Vertex{1, 0,   1,    0.65, 0.43, 0.3});

   mVertices.push_back(Vertex{1, 0,   1,    0.45,0.3,0.21});
   mVertices.push_back(Vertex{0, 0,   1,    0.45,0.3,0.21});
   mVertices.push_back(Vertex{0, 0,   0,    0.45,0.3,0.21});

   mMatrix.setToIdentity();
}

Plane::Plane(float x, float y, float s)
{
    mVertices.push_back(Vertex{0, 0,   0,          1,0,0});
    mVertices.push_back(Vertex{1, 0,   0,        0,1,0});
    mVertices.push_back(Vertex{1, 0,   1,          0,0,1});

    mVertices.push_back(Vertex{1, 0,   1,          1,1,1});
    mVertices.push_back(Vertex{0, 0,   1,       0.5,0.5,0.5});
    mVertices.push_back(Vertex{0, 0,   0,          0,0,0});

    mMatrix.setToIdentity();

    mMatrix.translate(x,y);
    mMatrix.scale(s);
    mMatrix.rotate(90,1,0,0);
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

    mMatrix.translate(-4.95f, -0.5f, -0.4f);
    mMatrix.rotate(90,0,0,1);

    //mMatrix.scale(1,1,1);
}

void Plane::OpenDoor()
{
    //mMatrix.rotate(-90, 0, 0, 1);
    bShouldOpen = true;
}

void Plane::draw()
{
    glBindVertexArray( mVAO );
    glUniformMatrix4fv( mMatrixUniform, 1, GL_FALSE, mMatrix.constData());
    glDrawArrays(GL_TRIANGLES, 0, mVertices.size());

    if (bShouldOpen)
    {
        i++;
        mMatrix.rotate(-1,0,0,1);
        if (i >= 90)
            bShouldOpen = false;
    }
}
