#ifndef INTERACTIVEOBJECT_H
#define INTERACTIVEOBJECT_H

#include "visualobject.h"
#include "CustomFiles/CustomVec2.h"

class InteractiveObject : public VisualObject {
public:
   InteractiveObject();
   ~InteractiveObject() override;
   void init(GLint matrixUniform) override;
   void draw() override;
   void initCubeGeometry();
   void move(float x, float y, float z) override;

   vec2f PlayerCoords;

private:
   float mx, my, mz; // posisjon
};

#endif // INTERACTIVEOBJECT_H
