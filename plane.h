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


};

#endif // PLANE_H
