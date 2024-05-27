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
#include <iostream>
// I think this could be a cool tool to use
// #define LOG(x) std::cout << x << std::endl

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

   // Maybe use other functions for these
   virtual QVector3D getPosition() { return mWorldPosition; }
   virtual QVector3D setPosition(float x, float y, float z) { return mWorldPosition = QVector3D{x,y,z}; }
   virtual float getRadius() { return radius; }

   bool bIsActive = true;
   bool bShouldRender = true;

   // Function meant for opening a door.
   virtual void OpenDoor() { }


   // Setters
   void SetRotation(float &angle, float &x, float &y, float &z) { mMatrix.rotate(angle, x, y, z); }
   void SetLocation(float &x, float &y, float &z) { mMatrix.translate(x,y,z); }
   void SetRotation(float angle, float x, float y, float z) { mMatrix.rotate(angle, x, y, z); }
   void SetLocation(float x, float y, float z) { mMatrix.translate(x,y,z); }

   // for phong lighting
   void calcAverageNormalsSmooth();
   void calcAverageNormalsFlat();


   const QVector3D &GetWorldPosition() const { return mWorldPosition; }
   void SetWorldPosition(QVector3D& newPosition){mWorldPosition += newPosition;}
   //TODO: Change this function
   void SetWorldPosition(float x,float y, float z) {mWorldPosition.setX(x);mWorldPosition.setY(y);mWorldPosition.setZ(z); }
   // Custom Matrix Functionalities
   QMatrix4x4 &GetMatrix() { ; return mMatrix; /*LOG("Calling Matrix") Maybe here you could print matrix or something like that*/}
   // TODO: Add custom "->" operator so we don't do mMatrix.translate, but rather GetMatrix()->Translate()
   QMatrix4x4 &TranslateMatrix(float &x, float&y, float&z){mMatrix.translate(x,y,z); }
   QMatrix4x4 &TranslateMatrix(float &&x, float&&y, float&&z){mMatrix.translate(x,y,z); }
   QVector3D &GetWorldPosition(){return mWorldPosition;} // TODO: MAKE THIS INTO CONST AND CREATE FUNCTIONS FOR SETTING AND GETTING
   // TODO: We are going to create getter and setter for the matrix and world position for more readability within this class.


    // TODO enable this
    // // Getter for bIsActive
    // bool isActive() const { return bIsActive; }

    // // Setter for bIsActive
    // void setActive(bool active) { bIsActive = active; }

    // // Getter for bShouldRender
    // bool shouldRender() const { return bShouldRender; }

    // // Setter for bShouldRender
    // void setShouldRender(bool render) { bShouldRender = render; }


protected:
   QVector3D mWorldPosition{};
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
};
#endif // VISUALOBJECT_H
