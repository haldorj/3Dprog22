#ifndef VISUALOBJECT_H
#define VISUALOBJECT_H

#include <QOpenGLFunctions_4_1_Core>
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
   virtual float getRadius() { return radius; }
   bool bIsActive = true;

   std::list<Vertex> pathNPC;
protected:
   float radius = 0;
   std::vector<Vertex> mVertices;
   GLuint mVAO{0};
   GLuint mVBO{0};
   GLint mMatrixUniform{0};
   QMatrix4x4 mMatrix;

   // Velger å lagre posisjon, rotasjon og translasjon
   // i hver sin 4x4 matrise
   QMatrix4x4 mPosition;
   QMatrix4x4 mRotation;
   QMatrix4x4 mScale;
   // Legger til rette for simulering
   QVector3D mVelocity{-0.01, 0, 0};
};
#endif // VISUALOBJECT_H
