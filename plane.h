#ifndef PLANE_H
#define PLANE_H
#include "visualobject.h"


class Plane : public VisualObject
{
public:
   Plane();
   ~Plane();

   void draw() override;
   void init(GLint matrixUniform);

   void SetRotation(float angle, float x, float y, float z);
   void SetLocation(float x, float y, float z);


};

#endif // PLANE_H
