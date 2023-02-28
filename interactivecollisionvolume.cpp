#include <iostream>
#include <sstream>
#include "interactivecollisionvolume.h"
#include <QVector3D>


//! \param n - the recursion level (default is 0 which makes the original Octahedron)
//!
//! The number of (duplicated) vertices are calculated based on the parameter n - the recursion level.
//! Initially we have 8 triangles, each with 3 vertices.
//! A subdivision consists of
//! - compute the midpoint of each edge in a triangel
//! - add these midpoint as vertices
//! - make three new edges by connecting new vertices
//! - preserve orientation
//! - move the three new points on the unit ball's surface by normalizing. Remember the vertex vector
//! is also a normal to the ball's surface.
//! The formula for the number of vertices is
//! m_nVertices = 3 * 8 * 4^n
//! where
//! - 3 comes from the number of vertices in a triangle,
//! - 8 comes from the original number of triangles in a regular Octahedron
//! - n is the recursion level (number of repeated subdivisions)
//!


InteractiveCollisionVolume::InteractiveCollisionVolume(int n) : m_rekursjoner(n), m_indeks(0), VisualObject()
{
   radius = 0.4;
   mVertices.reserve(3 * 8 * pow(4, m_rekursjoner));
   oktaederUnitBall();

   mWorldPosition = {0.0f,0.0f,0.0f};
   mMatrix.translate(-0.1, 0.1, 0);
   mMatrix.scale(radius);
}

//!//! \brief OctahedronBall::~OctahedronBall() virtual destructor
//!
InteractiveCollisionVolume::~InteractiveCollisionVolume()
{
   //
}


// Parametre inn: xyz koordinatene til et triangle v1, v2, v3 ccw
// Bevarer orienteringen av hjørnene
//!
//! \brief OctahedronBall::lagTriangel()
//! \param v1 - position on the unit ball for vertex 1
//! \param v2 - position on the unit ball for vertex 2
//! \param v3 - position on the unit ball for vertex 3
//!
//! lagTriangel() creates vertex data for a triangle's 3 vertices. This is done in the
//! final step of recursion.
//!
void InteractiveCollisionVolume::lagTriangel(const QVector3D& v1, const QVector3D& v2,
const QVector3D& v3)
{
    Vertex v{v1.x(), v1.y(), v1.z(), 1, 1, 1};
     mVertices.push_back(v);
     v = Vertex{v2.x(), v2.y(), v2.z(), 1, 1, 1};
     mVertices.push_back(v);
     v = Vertex{v3.x(), v3.y(), v3.z(), 1, 1, 1};
     mVertices.push_back(v);
}


  // Rekursiv subdivisjon av triangel
  //!
  //! \brief OctahedronBall::subDivide() recursive subdivision of a triangel
  //! \param a coordinates for vertex a
  //! \param b coordinates for vertex b
  //! \param c coordinates for vertex c
  //! \param n level of recursion
  //!
  //! The function tests
  //! (if n>0)
  //! - three new edge midpoints are computed and normalized,
  //! - four new calls to subDivide() is done - one for each new triangle, preserving orientation
  //! and with decreased parameter n
  //! else
  //! - call lagTriangel(a, b, c)
  //!
  void InteractiveCollisionVolume::subDivide(const QVector3D& a, const QVector3D& b, const QVector3D& c, int n)
  {
     if (n>0) {
         QVector3D v1 = a+b; v1.normalize();
         QVector3D v2 = a+c; v2.normalize();
         QVector3D v3 = c+b; v3.normalize();
         subDivide(a, v1, v2, n-1);
         subDivide(c, v2, v3, n-1);
         subDivide(b, v3, v1, n-1);
         subDivide(v3, v2, v1, n-1);
     } else {
         lagTriangel(a, b, c);
     }
  }


  //!
  //! \brief OctahedronBall::oktaederUnitBall() creates 8 unit ball vertices and call subDivide()
  //!
  //! If the parameter n of the constructor OctahedronBall() is zero, the result will be the
  //! original octahedron consisting of 8 triangles with duplicated vertices.
  //!
  void InteractiveCollisionVolume::oktaederUnitBall()
  {
     QVector3D v0{0, 0, 1};
     QVector3D v1{1, 0, 0};
     QVector3D v2{0, 1, 0};
     QVector3D v3{-1, 0, 0};
     QVector3D v4{0, -1, 0};
     QVector3D v5{0, 0, -1};


     subDivide(v0, v1, v2, m_rekursjoner);
     subDivide(v0, v2, v3, m_rekursjoner);
     subDivide(v0, v3, v4, m_rekursjoner);
     subDivide(v0, v4, v1, m_rekursjoner);
     subDivide(v5, v2, v1, m_rekursjoner);
     subDivide(v5, v3, v2, m_rekursjoner);
     subDivide(v5, v4, v3, m_rekursjoner);
     subDivide(v5, v1, v4, m_rekursjoner);
  }


  //!
  //! \brief OctahedronBall::initVertexBufferObjects() calls glGenBuffers(), glBindBuffer() and glBufferdata()
  //! for using later use of glDrawArrays()
  //!
  void InteractiveCollisionVolume::init(GLint matrixUniform)
  {
     mMatrixUniform = matrixUniform;
     initializeOpenGLFunctions();


     //Vertex Array Object - VAO
     glGenVertexArrays( 1, &mVAO );
     glBindVertexArray( mVAO );


     //Vertex Buffer Object to hold vertices - VBO
     glGenBuffers( 1, &mVBO );
     glBindBuffer( GL_ARRAY_BUFFER, mVBO );


     glBufferData( GL_ARRAY_BUFFER, mVertices.size()*sizeof(Vertex), mVertices.data(), GL_STATIC_DRAW );


     glBindBuffer(GL_ARRAY_BUFFER, mVBO);
     glVertexAttribPointer(0, 3, GL_FLOAT,GL_FALSE,sizeof(Vertex), (GLvoid*)0);
     glEnableVertexAttribArray(0);


     glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,  sizeof(Vertex),  (GLvoid*)(3 * sizeof(GLfloat)) );
     glEnableVertexAttribArray(1);
     glBindVertexArray(0);
  }

  //!
  //! \brief OctahedronBall::draw() draws a ball using glDrawArrays()
  //! \param positionAttribute    vertex shader variable for position
  //! \param normalAttribute      vertex shader variable for normal or color
  //! \param textureAttribute     vertex shader variable for texture coordinates (optional)
  //!
  //! draw()
  //! - glBindBuffer()
  //! - glVertexAttribPointer()
  //! - glBindTexture()
  //! - glVertexAttribPointer()
  //! - glDrawArrays() with GL_TRIANGLES
  //!
  void InteractiveCollisionVolume::draw()
  {
     glBindVertexArray( mVAO );
     glUniformMatrix4fv( mMatrixUniform, 1, GL_FALSE, mMatrix.constData());
     glDrawArrays(GL_LINES, 0, mVertices.size());//mVertices.size());
  }


  void InteractiveCollisionVolume::move(float x, float y, float z)
  {
      mx += x;
      my += y;
      mz += z;
      mMatrix.translate(mx, my, mz);
      mx =0;
      my =0;
      mz =0;
  }
