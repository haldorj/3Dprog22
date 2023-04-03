#ifndef BSPLINECURVE_H
#define BSPLINECURVE_H

#include "visualobject.h"

class BSplineCurve : public VisualObject
{
public:
    BSplineCurve();
    int d; // graden på kurven
    float t[10]; // Skjøtvektor {0,0,0,1,2,3,4,5,5,5}
    QVector3D c[7];
    int findKnotInterval(float t);
    QVector3D deBoor(float t);
};

#endif // BSPLINECURVE_H
