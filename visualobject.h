#ifndef VISUALOBJECT_H
#define VISUALOBJECT_H

#include <QOpenGLFunctions_4_1_Core>
#include <glm.hpp>
#include <QMatrix4x4>
#include <QMatrix2x2>
#include <vector>
#include <list>
#include <QVector3D>
#include "vertex.h"

class VisualObject : public QOpenGLFunctions_4_1_Core {
public:
   VisualObject();
   ~VisualObject();
   virtual void init(GLint matrixUniform)=0;
   virtual void draw()=0;
   // move for interaktive objekter
   virtual void move(float x, float y, float z) { }
   // move for simulering (for eksempel NPC)
   virtual void move(float dt) { }

   QVector3D mWorldPosition{};
   virtual QVector3D getPosition() { return mWorldPosition; }
   virtual QVector3D setPosition(float x, float y, float z) { mWorldPosition = QVector3D{x,y,z}; }
   virtual float getRadius() { return radius; }

   bool bIsActive = true;
   bool bShouldRender = true;

   // Function meant for opening a door.
   virtual void OpenDoor() { }

   void SetRotation(float angle, float x, float y, float z) { mMatrix.rotate(angle, x, y, z); }
   void SetLocation(float x, float y, float z) { mMatrix.translate(x,y,z); }

   // for phong lighting
   void calcAverageNormals();

   QMatrix4x4 mMatrix;
protected:
   float radius = 0;
   std::vector<Vertex> mVertices;
   std::vector<unsigned int> mIndices;
   GLuint mVAO{0};
   GLuint mVBO{0};
   GLuint mIBO{0};
   GLint mMatrixUniform{0};

   // Velger å lagre posisjon, rotasjon og translasjon
   // i hver sin 4x4 matrise
   QMatrix4x4 mPosition;
   QMatrix4x4 mRotation;
   QMatrix4x4 mScale;
   // Legger til rette for simulering
   QVector3D mVelocity{  };
};
#endif // VISUALOBJECT_H
