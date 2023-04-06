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

   glm::vec3 getVertex(int index);
   unsigned int getIndex(int triangleIndex, int vertexIndex);

   void initIndeces();
   int numTriangles{};
};


#endif // TRIANGULATION_H
