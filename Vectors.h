#ifndef VECTORS_H
#define VECTORS_H

struct Vec2
{
    float x;
    float y;

    Vec2()
    {
        x = 0;
        y = 0;
    }

    Vec2(float in_x, float in_y)
    {
        x = in_x;
        y = in_y;
    }

    Vec2 operator+=( const Vec2& v ) {
        this->x += v.x;
        this->y += v.y;

        return *this;
    }

    Vec2 operator-=( const Vec2& v ) {
        this->x -= v.x;
        this->y -= v.y;

        return *this;
    }

    // add two vectors together like vec3 c = a + b;
    Vec2 operator+( const Vec2& a , const Vec2& b ) {
        return Vec2( a.x + b.x , a.y + b.y);
    }
};



#endif // VECTORS_H
