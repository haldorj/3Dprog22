#ifndef INTERACTIVEOBJECT_H
#define INTERACTIVEOBJECT_H

#include <glm/glm.hpp>

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


   glm::vec3 barycentricCoordinates(const glm::vec2& p1, const glm::vec2& p2, const glm::vec2& p3);
   glm::vec2 PlayerCoords;

private:
   float mx, my, mz; // posisjon
};

#endif // INTERACTIVEOBJECT_H
