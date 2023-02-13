#ifndef TRIANGLESURFACE_H
#define TRIANGLESURFACE_H

#include <fstream>
#include <iostream>
#include "visualobject.h"

class TriangleSurface : public VisualObject
{
public:
   TriangleSurface();
   TriangleSurface(std::string filnavn);
   ~TriangleSurface() override;

   void constructFrankes();
   void readFile(std::string filename);
   void writeFile(std::string filename);
   void init(GLint matrixUniform) override;
   void constructOblig1Oppg3Math();
   void draw() override;
};

#endif // TRIANGLESURFACE_H
