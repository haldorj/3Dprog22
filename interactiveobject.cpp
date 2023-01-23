#include "interactiveobject.h"

InteractiveObject::InteractiveObject() : mx{0.0f}, my{0.0f}, mz{0.0f}
{
    mVertices.push_back(Vertex{ -0.25f,-0.25f,-0.25f, 0.583f,  0.771f,  0.014f});
    mVertices.push_back(Vertex{-0.25f,-0.25f, 0.25f, 0.609f,  0.115f,  0.436f});
    mVertices.push_back(Vertex{-0.25f, 0.25f, 0.25f, 0.327f,  0.483f,  0.844f});
    mVertices.push_back(Vertex{0.25f, 0.25f,-0.25f, 0.822f,  0.569f,  0.201f});
    mVertices.push_back(Vertex{-0.25f,-0.25f,-0.25f, 0.435f,  0.602f,  0.223f});
    mVertices.push_back(Vertex{-0.25f, 0.25f,-0.25f, 0.310f,  0.747f,  0.185f});
    mVertices.push_back(Vertex{0.25f,-0.25f, 0.25f, 0.597f,  0.770f,  0.761f});
    mVertices.push_back(Vertex{-0.25f,-0.25f,-0.25f, 0.559f,  0.436f,  0.730f});
    mVertices.push_back(Vertex{0.25f,-0.25f,-0.25f, 0.359f,  0.583f,  0.152f});
    mVertices.push_back(Vertex{0.25f, 0.25f,-0.25f, 0.483f,  0.596f,  0.789f});
    mVertices.push_back(Vertex{0.25f,-0.25f,-0.25f, 0.559f,  0.861f,  0.639f});
    mVertices.push_back(Vertex{-0.25f,-0.25f,-0.25f, 0.195f,  0.548f,  0.859f});
    mVertices.push_back(Vertex{-0.25f,-0.25f,-0.25f, 0.014f,  0.184f,  0.576f});
    mVertices.push_back(Vertex{-0.25f, 0.25f, 0.25f, 0.771f,  0.328f,  0.970f});
    mVertices.push_back(Vertex{-0.25f, 0.25f,-0.25f, 0.406f,  0.615f,  0.116f});
    mVertices.push_back(Vertex{0.25f,-0.25f, 0.25f, 0.676f,  0.977f,  0.133f});
    mVertices.push_back(Vertex{-0.25f,-0.25f, 0.25f, 0.971f,  0.572f,  0.833f});
    mVertices.push_back(Vertex{-0.25f,-0.25f,-0.25f, 0.140f,  0.616f,  0.489f});
    mVertices.push_back(Vertex{-0.25f, 0.25f, 0.25f, 0.997f,  0.513f,  0.064f});
    mVertices.push_back(Vertex{-0.25f,-0.25f, 0.25f, 0.945f,  0.719f,  0.592f});
    mVertices.push_back(Vertex{0.25f,-0.25f, 0.25f, 0.543f,  0.021f,  0.978f});
    mVertices.push_back(Vertex{0.25f, 0.25f, 0.25f, 0.279f,  0.317f,  0.505f});
    mVertices.push_back(Vertex{0.25f,-0.25f,-0.25f, 0.167f,  0.620f,  0.077f});
    mVertices.push_back(Vertex{0.25f, 0.25f,-0.25f, 0.347f,  0.857f,  0.137f});
    mVertices.push_back(Vertex{0.25f,-0.25f,-0.25f, 0.055f,  0.953f,  0.042f});
    mVertices.push_back(Vertex{0.25f, 0.25f, 0.25f, 0.714f,  0.505f,  0.345f});
    mVertices.push_back(Vertex{0.25f,-0.25f, 0.25f, 0.783f,  0.290f,  0.734f});
    mVertices.push_back(Vertex{0.25f, 0.25f, 0.25f, 0.722f,  0.645f,  0.174f});
    mVertices.push_back(Vertex{0.25f, 0.25f,-0.25f, 0.302f,  0.455f,  0.848f});
    mVertices.push_back(Vertex{-0.25f, 0.25f,-0.25f, 0.225f,  0.587f,  0.040f});
    mVertices.push_back(Vertex{0.25f, 0.25f, 0.25f, 0.517f,  0.713f,  0.338f});
    mVertices.push_back(Vertex{-0.25f, 0.25f,-0.25f, 0.053f,  0.959f,  0.120f});
    mVertices.push_back(Vertex{-0.25f, 0.25f, 0.25f, 0.393f,  0.621f,  0.362f});
    mVertices.push_back(Vertex{0.25f, 0.25f, 0.25f, 0.673f,  0.211f,  0.457f});
    mVertices.push_back(Vertex{-0.25f, 0.25f, 0.25f, 0.820f,  0.883f,  0.371f});
    mVertices.push_back(Vertex{0.25f,-0.25f, 0.25f, 0.820f,  0.883f,  0.371f});
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

void InteractiveObject::draw()
{
    glBindVertexArray( mVAO );
    glUniformMatrix4fv( mMatrixUniform, 1, GL_FALSE, mMatrix.constData());
    glDrawArrays(GL_TRIANGLES, 0, mVertices.size());
    mMatrix.rotate(1.0f, 1.0f, 2.0f, 0.0f);
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
