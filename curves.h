#ifndef CURVES_H
#define CURVES_H

#include <fstream>
#include <iostream>
#include "Eigen/Dense"

#include "visualobject.h"

class Curves : public VisualObject
{
public:
    Curves();
    Curves(std::string filnavn);
    ~Curves();

    void constructCurve();
    void calculateCurveTask1();
    void calculateCurveTask1Parabel();
    void calculateCurve2();
    void writeFile(std::string filename);
    void readFile(std::string filename);

    void draw() override;
    void init(GLint matrixUniform) override;

private:
    float mX{};
    float mY{};

    float mA = 0;
    float mB = 0;
    float mC = 0;
    float mD = 0;
};

#endif // CURVES_H
