#ifndef PLANE_H
#define PLANE_H
#include "visualobject.h"


class Plane : public VisualObject
{
public:
   Plane();
   Plane(float x, float y, float s);
   ~Plane();

   void draw() override;
   void init(GLint matrixUniform);

   void OpenDoor();

private:
   int i = 0;
   bool bShouldOpen = false;
};

#endif // PLANE_H
