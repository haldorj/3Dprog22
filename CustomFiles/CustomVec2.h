#ifndef CUSTOMVEC2_H
#define CUSTOMVEC2_H

#include <cmath>
#include "CustomVec3.h"

template <class T>
class Vec2 {
public:
    T x, y;

    Vec2() :x(0), y(0) {}
    Vec2(T x, T y) : x(x), y(y) {}
    Vec2(const Vec2& v) : x(v.x), y(v.y) {}

    Vec2& operator=(const Vec2& v) {
        x = v.x;
        y = v.y;
        return *this;
    }

    Vec2 operator+(Vec2& v) {
        return Vec2(x + v.x, y + v.y);
    }
    Vec2 operator-(Vec2& v) {
        return Vec2(x - v.x, y - v.y);
    }

    Vec2& operator+=(Vec2& v) {
        x += v.x;
        y += v.y;
        return *this;
    }
    Vec2& operator-=(Vec2& v) {
        x -= v.x;
        y -= v.y;
        return *this;
    }

    Vec2 operator+(double s) {
        return Vec2(x + s, y + s);
    }
    Vec2 operator-(double s) {
        return Vec2(x - s, y - s);
    }
    Vec2 operator*(double s) {
        return Vec2(x * s, y * s);
    }
    Vec2 operator/(double s) {
        return Vec2(x / s, y / s);
    }


    Vec2& operator+=(double s) {
        x += s;
        y += s;
        return *this;
    }
    Vec2& operator-=(double s) {
        x -= s;
        y -= s;
        return *this;
    }
    Vec2& operator*=(double s) {
        x *= s;
        y *= s;
        return *this;
    }
    Vec2& operator/=(double s) {
        x /= s;
        y /= s;
        return *this;
    }

    void set(T x, T y) {
        this->x = x;
        this->y = y;
    }

    void rotate(double deg) {
        double theta = deg / 180.0 * M_PI;
        double c = cos(theta);
        double s = sin(theta);
        double tx = x * c - y * s;
        double ty = x * s + y * c;
        x = tx;
        y = ty;
    }

    Vec2& normalize() {
        if (length() == 0) return *this;
        *this *= (1.0 / length());
        return *this;
    }

    float dist(Vec2 v) const {
        Vec2 d(v.x - x, v.y - y);
        return d.length();
    }
    float length() const {
        return std::sqrt(x * x + y * y);
    }
    void truncate(double length) {
        double angle = atan2f(y, x);
        x = length * cos(angle);
        y = length * sin(angle);
    }

    Vec2 ortho() const {
        return Vec2(y, -x);
    }

    static float dot(Vec2 v1, Vec2 v2) {
        return v1.x * v2.x + v1.y * v2.y;
    }
    static float cross(Vec2 v1, Vec2 v2) {
        return (v1.x * v2.y) - (v1.y * v2.x);
    }

    Vec3 barycentricCoordinates(const Vec2& p1, const Vec2& p2, const Vec2& p3)
    {
        Vec2 p12 = p2-p1;
        Vec2 p13 = p3-p1;
        Vec3 n = p12^p13;
        float areal_123 = n.length(); // dobbelt areal
        Vec3 baryc; // til retur. Husk
        // u
        Vec2 p = p2 - *this;
        Vec2 q = p3 - *this;
        n = p^q;
        baryc.x = n.z/areal_123;
        // v
        p = p3 - *this;
        q = p1 - *this;
        n = p^q;
        baryc.y = n.z/areal_123;
        // w
        p = p1 - *this;
        q = p2 - *this;
        n = p^q;
        baryc.z = n.z/areal_123;
        return baryc;
    }
};

typedef Vec2<float> vec2f;
typedef Vec2<double> vec2d;

#endif // CUSTOMVEC2_H
