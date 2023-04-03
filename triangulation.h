#ifndef TRIANGULATION_H
#define TRIANGULATION_H

#include <vector>

#include <glm/glm.hpp>

#include "visualobject.h"
#include "vertex.h"

class Triangulation : public VisualObject
{
public:
   Triangulation();
   Triangulation(std::string filnavn);
   ~Triangulation() override;

   void readFile(std::string filename);
   void writeFile(std::string filename);
   void init(GLint matrixUniform) override;
   void draw() override;

   // Get the vertices of the triangles.
   void GetTriangleVertices();
   // Stores all corners of each triangle in the surface
   std::vector<glm::vec2> mTriangleVertices;

   glm::vec3 getVertex(int index);
   unsigned int getIndex(int triangleIndex, int vertexIndex);

   int numTriangles = 14;
};


#endif // TRIANGULATION_H
