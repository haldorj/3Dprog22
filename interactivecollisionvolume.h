#ifndef INTERACTIVECOLLISIONVOLUME_H
#define INTERACTIVECOLLISIONVOLUME_H

#include "visualobject.h"

class InteractiveCollisionVolume : public VisualObject {
private:
   int m_rekursjoner;
   int m_indeks;               // brukes i rekursjon, til å bygge m_vertices
   void lagTriangel(const QVector3D& v1, const QVector3D& v2, const QVector3D& v3);
   void subDivide(const QVector3D& a, const QVector3D& b, const QVector3D& c, int n);
   void oktaederUnitBall();
   void move(float x, float y, float z) override;
public:
   InteractiveCollisionVolume(int n=0);
   ~InteractiveCollisionVolume();
   void init(GLint matrixUniform);
   void draw();
   float mx, my, mz; // posisjon

   float radius{};
};

#endif // COLLISIONVOLUME_H
