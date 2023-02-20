#ifndef COLLISIONVOLUME_H
#define COLLISIONVOLUME_H

#include "visualobject.h"

class CollisionVolume : public VisualObject {
private:
   int m_rekursjoner;
   int m_indeks;               // brukes i rekursjon, til å bygge m_vertices
   void lagTriangel(const QVector3D& v1, const QVector3D& v2, const QVector3D& v3);
   void subDivide(const QVector3D& a, const QVector3D& b, const QVector3D& c, int n);
   void oktaederUnitBall();
public:
   CollisionVolume(int n=0);
   CollisionVolume(float x, float y, int n=0);
   ~CollisionVolume();
   void init(GLint matrixUniform);
   void draw();
   void move(float x, float y, float z) override;

   bool bIsActive = true;
};

#endif // COLLISIONVOLUME_H
