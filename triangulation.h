#ifndef TRIANGULATION_H
#define TRIANGULATION_H

#include <vector>

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

   std::vector<Vertex> GetTriangles();

   // Stores all corners of each triangle in the surface
   std::vector<Vertex> TriangleCorners;
};


#endif // TRIANGULATION_H
