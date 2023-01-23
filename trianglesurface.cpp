#include "trianglesurface.h"

TriangleSurface::TriangleSurface() : VisualObject()
{
  //                x    y     z   r g b
  Vertex v0{0.0,0.0,0.0, 1,0,0};    mVertices.push_back(v0);
  Vertex v1(0.5,0.0,0.0, 0,1,0);    mVertices.push_back(v1);
  Vertex v2{0.5,0.5,0.0, 0,0,1};    mVertices.push_back(v2);
  Vertex v3{0.0,0.0,0.0, 0,0,1};    mVertices.push_back(v3);
  Vertex v4{0.5,0.5,0.0, 0,1,0};    mVertices.push_back(v4);
  Vertex v5{0.0,0.5,0.0, 1,0,0};    mVertices.push_back(v5);


  mMatrix.setToIdentity();
}

TriangleSurface::TriangleSurface(std::string filnavn) : VisualObject()
{
   readFile(filnavn);
   mMatrix.setToIdentity();
}

TriangleSurface::~TriangleSurface()
{

}

void TriangleSurface::readFile(std::string filnavn) {
   std::ifstream inn;
   inn.open(filnavn.c_str());


   if (inn.is_open()) {
       int n;
       Vertex vertex;
       inn >> n;
       mVertices.reserve(n);
       for (int i=0; i<n; i++) {
            inn >> vertex;
            mVertices.push_back(vertex);
       }
       inn.close();
   }
}

void TriangleSurface::init(GLint matrixUniform)
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

void TriangleSurface::draw()
{
    glBindVertexArray( mVAO );
    glUniformMatrix4fv( mMatrixUniform, 1, GL_FALSE, mMatrix.constData());
    glDrawArrays(GL_TRIANGLES, 0, mVertices.size());
}
