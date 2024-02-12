#include "cube.h"

Cube::Cube()
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

    mMatrix.setToIdentity();
}

Cube::Cube(float size,float x, float y, float z, float r, float g, float b)
{
    /*bottom from drawing*/
    Vertex v0{x,y,z,                    r,g,b};    mVertices.push_back(v0);
    Vertex v1{x,y+size,z,               r,g,b};    mVertices.push_back(v1);
    Vertex v2{x+size,y+size,z,          r,g,b};    mVertices.push_back(v2);

    Vertex v3{ x,y,z,                   r,g,b };    mVertices.push_back(v3);
    Vertex v4{ x+size,y,z,              r,g,b };    mVertices.push_back(v4);
    Vertex v5{ x+size,y+size,z,         r,g,b };    mVertices.push_back(v5);


    /* left of drawing */
    Vertex v6{x,y,z,                    r,g,b};    mVertices.push_back(v6);
    Vertex v7{x+size,y,z,               r,g,b};    mVertices.push_back(v7);
    Vertex v8{x+size,y,z+size,          r,g,b};    mVertices.push_back(v8);

    Vertex v9{ x,y,z,                   r,g,b };    mVertices.push_back(v9);
    Vertex v10{ x,y,z+size,             r,g,b }; mVertices.push_back(v10);
    Vertex v11{ x+size,y,z+size,        r,g,b };   mVertices.push_back(v11);

    /*right of drawing */
    Vertex v12{ x,y+size,z,             r,g,b };    mVertices.push_back(v12);
    Vertex v13{ x+size,y+size,z,        r,g,b };    mVertices.push_back(v13);
    Vertex v14{ x+size,y+size,z+size,   r,g,b };    mVertices.push_back(v14);

    Vertex v15{ x+size,y+size,z+size,   r,g,b };    mVertices.push_back(v15);
    Vertex v16{ x,y+size,z+size,        r,g,b };    mVertices.push_back(v16);
    Vertex v17{ x,y+size,z,             r,g,b };    mVertices.push_back(v17);

    /* front of drawing */
    Vertex v18{ x+size,y,z,             r,g,b };    mVertices.push_back(v18);
    Vertex v19{ x+size,y+size,z,        r,g,b };    mVertices.push_back(v19);
    Vertex v20{ x+size,y+size,z+size,   r,g,b };    mVertices.push_back(v20);

    Vertex v21{ x+size,y,z,             r,g,b };    mVertices.push_back(v21);
    Vertex v22{ x+size,y,z+size,        r,g,b };    mVertices.push_back(v22);
    Vertex v23{ x+size,y+size,z+size,   r,g,b };    mVertices.push_back(v23);

    /* behind of drawing*/
    Vertex v24{ x,y,z,                  r,g,b };    mVertices.push_back(v24);
    Vertex v25{ x,y+size,z,             r,g,b };    mVertices.push_back(v25);
    Vertex v26{ x,y+size,z+size,        r,g,b };    mVertices.push_back(v26);

    Vertex v27{ x,y,z,                  r,g,b };    mVertices.push_back(v27);
    Vertex v28{ x,y,z+size,             r,g,b };    mVertices.push_back(v28);
    Vertex v29{ x,y+size,z+size,        r,g,b };    mVertices.push_back(v29);

    /* above of drawing */
    Vertex v30{ x,y,z+size,             r,g,b };    mVertices.push_back(v30);
    Vertex v31{ x+size,y,z+size,        r,g,b };    mVertices.push_back(v31);
    Vertex v32{ x+size,y+size,z+size,   r,g,b };    mVertices.push_back(v32);

    Vertex v33{ x+size,y+size,z+size,   r,g,b };    mVertices.push_back(v33);
    Vertex v34{ x,y+size,z+size,        r,g,b };    mVertices.push_back(v34);
    Vertex v35{ x,y,z+size,             r,g,b };    mVertices.push_back(v35);

    mMatrix.setToIdentity();
}

Cube::~Cube()
{

}

void Cube::init(GLint matrixUniform)
{
    mMatrixUniform = matrixUniform;
    initializeOpenGLFunctions();

    glGenVertexArrays(1, &mVAO);
    glBindVertexArray(mVAO);

    glGenBuffers(1, &mVBO);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);

    glBufferData( GL_ARRAY_BUFFER, mVertices.size()*sizeof(Vertex), mVertices.data(), GL_STATIC_DRAW );

    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<GLvoid*>(0));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,  sizeof(Vertex),  reinterpret_cast<GLvoid*>(3 * sizeof(GLfloat)) );
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);
}

void Cube::draw()
{
    glBindVertexArray( mVAO );
    glUniformMatrix4fv( mMatrixUniform, 1, GL_FALSE, mMatrix.constData());

    if (bShouldRender)
        glDrawArrays(GL_TRIANGLES, 0, mVertices.size());
    else
        glDrawArrays(GL_TRIANGLES, 0, 0);
}
