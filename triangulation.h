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

   void calcAverageNormals(unsigned int* indices, unsigned int indexCount, GLfloat* vertices, unsigned int vertexCount,
       unsigned int vLength, unsigned int normalOffset);

   int numTriangles = 14;

   GLfloat vertices[96] = {

  // x,   y,   z,     u,   v,   nx, ny, nz
   0.0, 0.0, 2.0,   1.0, 1.0,   1.0,0.0,0.0,
   0.0, 1.0, 2.0,   1.0, 1.0,   1.0,0.0,0.0,
   0.0, 4.0, 2.0,   1.0, 1.0,   1.0,0.0,0.0,
   1.0, 3.0, 1.0,   1.0, 1.0,   1.0,0.0,0.0,
   1.5, 0.0, 2.0,   1.0, 1.0,   1.0,0.0,0.0,
   2.0, 2.0, 2.0,   1.0, 1.0,   1.0,0.0,0.0,
   2.5, 1.0, 1.0,   1.0, 1.0,   1.0,0.0,0.0,
   2.5, 4.0, 2.0,   1.0, 1.0,   1.0,0.0,0.0,
   3.5, 2.5, 1.0,   1.0, 1.0,   1.0,0.0,0.0,
   4.0, 0.0, 2.0,   1.0, 1.0,   1.0,0.0,0.0,
   4.0, 0.5, 2.0,   1.0, 1.0,   1.0,0.0,0.0,
   4.0, 4.0, 2.0,   1.0, 1.0,   1.0,0.0,0.0

   };


   unsigned int mIndices[50] = {
       0, 4, 1,    // 1. trekant
       1, 4, 6,    // 2. trekant
       1, 6, 5,    // 3. osv...
       1, 5, 3,
       1, 3, 2,
       4, 10, 6,
       4, 9, 10,
       8, 10, 11,
       5, 6, 10,
       5, 10, 8,
       7, 8, 11,
       5, 8, 7,
       3, 5, 7,
       2, 3, 7
   };

};


#endif // TRIANGULATION_H
