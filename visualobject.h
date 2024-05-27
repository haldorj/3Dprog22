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
   virtual void move(float &x, float &y,float &z) {x = 0; y = 0;z = 0; } /*Just added this so we don't get an error message in Qt*/
   virtual void move(float x, float y,float z) {_mx += x; _my += y;_mz += z; mMatrix.translate(_mx, _my, _mz); _mx =0; _my =0; _mz =0; } /*Just added this so we don't get an error message in Qt*/
   // move for simulering (for eksempel NPC)
   virtual void move(float dt) {  }

   // TODO: check if these getters and setters are reduntant since they are virtual.
   virtual QVector3D getPosition() { return mWorldPosition; }
   virtual QVector3D setPosition(float x, float y, float z) { mWorldPosition = QVector3D{x,y,z}; }
   virtual float getRadius() { return radius; }

   bool bIsActive = true;
   bool bShouldRender = true;

   // Function meant for opening a door.
   virtual void OpenDoor() { }

   void SetRotation(float &angle, float &x, float &y, float &z) { mMatrix.rotate(angle, x, y, z); }
   void SetLocation(float &x, float &y, float &z) { mMatrix.translate(x,y,z); }

   //void SetRotation(float angle, float x, float y, float z) { mMatrix.rotate(angle, x, y, z); }
   //void SetLocation(float x, float y, float z) { mMatrix.translate(x,y,z); }

   // for phong lighting
   void calcAverageNormalsSmooth();
   void calcAverageNormalsFlat();

   // Trying to fix readability

   // Custom Matrix Functionalities
   QMatrix4x4 &GetMatrix(){return mMatrix;}
   QMatrix4x4 &TranslateMatrix(float &x, float&y, float&z){mMatrix.translate(x,y,z); }   // TODO: Add custom "->" operator so we don't do mMatrix.translate, but rather GetMatrix()->Translate()
   QMatrix4x4 &TranslateMatrix(float &&x, float&&y, float&&z){mMatrix.translate(x,y,z); }
   const QVector3D &GetWorldPosition(){return mWorldPosition;}


   QVector3D &SetWorldPosition(float x,float y,float z){mWorldPosition.setX(x); mWorldPosition.setX(y);mWorldPosition.setX(z); return mWorldPosition; }
   // QVector3D &SetWorldPosition(float x,float y,float z){mWorldPosition.x() + x; mWorldPosition.y() + y;mWorldPosition.z() + z; }
   QMatrix4x4 &RotateMatrix(float &angle, float &x,float&y,float&z) {mMatrix.rotate(angle, x, y, z = 0.0f); }
   QMatrix4x4 &RotateMatrix(float &&angle, float &&x,float&&y,float&&z) {mMatrix.rotate(angle, x, y, z = 0.0f); }

   QVector3D mWorldPosition{};
protected:

    QMatrix4x4 mMatrix;

   float radius = 0;
   std::vector<Vertex> mVertices;
   std::vector<unsigned int> mIndices;
   GLuint mVAO{0}; // Vertex Array Object
   GLuint mVBO{0}; // Vertex Buffer Object
   GLuint mIBO{0}; // Index Buffer Object
   GLint mMatrixUniform{0};

   // Velger å lagre posisjon, rotasjon og translasjon
   // i hver sin 4x4 matrise
   QMatrix4x4 mPosition;
   QMatrix4x4 mRotation;
   QMatrix4x4 mScale;
   // Legger til rette for simulering
   QVector3D mVelocity{  };

   float _mx, _my, _mz; // posisjon

};
#endif // VISUALOBJECT_H

// Kinda want to put these in a namespace or something later.
// QVector3D &SetWorldPosition(float&&x,float&y,float&z){mWorldPosition.setX(x); mWorldPosition.setX(y);mWorldPosition.setX(z); }
// QVector3D &SetWorldPosition(float&x,float&&y,float&z){mWorldPosition.setX(x); mWorldPosition.setX(y);mWorldPosition.setX(z); }
// QVector3D &SetWorldPosition(float&&x,float&y,float&&z){mWorldPosition.setX(x); mWorldPosition.setX(y);mWorldPosition.setX(z); }
// QVector3D &SetWorldPosition(float x,float y,float z){mWorldPosition.setX(x); mWorldPosition.setX(y);mWorldPosition.setX(z); }
// QVector3D &SetWorldPosition(float&&x,float&&y,float&&z){mWorldPosition.setX(x); mWorldPosition.setX(y);mWorldPosition.setX(z); }
// QVector3D &SetWorldPosition(float&x,float&y,float&z){mWorldPosition.setX(x); mWorldPosition.setX(y);mWorldPosition.setX(z); }


