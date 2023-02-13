#ifndef CURVES_H
#define CURVES_H

#include <fstream>
#include <iostream>
#include "visualobject.h"

class Curves : public VisualObject
{
public:
    Curves();
    ~Curves();

    void DoCurve();
    void writeFile(std::string filename);
    void readFile(std::string filename);

    void draw() override;
    void init(GLint matrixUniform) override;
    void move(float dx, float dy, float dz) override;
};

#endif // CURVES_H
