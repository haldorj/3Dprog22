#ifndef CUSTOMVEC3_H
#define CUSTOMVEC3_H

#include <cmath>

struct Vec3
{
    float x;
    float y;
    float z;

public:
    // Cross product
    Vec3 operator ^ (const Vec3& v) const
    {
        Vec3 w;
        w.x =  0.0;
        w.y =  0.0;
        w.z =  x * v.y - y * v.x;
        return w;
    }

    float length() const {
        return std::sqrt(x*x + y*y + z*z);
    }
};

#endif // CUSTOMVEC3_H
