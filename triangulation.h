#ifndef TRIANGULATION_H
#define TRIANGULATION_H

#include "visualobject.h"

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
};


#endif // TRIANGULATION_H
