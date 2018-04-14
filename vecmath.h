#pragma once

#include <math.h>

template<class T>
class Vec2
{
public:
    T x, y;

    Vec2() {}

    Vec2(const Vec2& v)
    {
        *this = v;
    }

    Vec2(const T& x, const T& y) : x(x), y(y) {}

    const Vec2& operator=(const Vec2& v)
    {
        x = v.x;
        y = v.y;
        return *this;
    }

    Vec2 operator+(const Vec2& v)
    {
        return Vec3(x + v.x, y + v.y);
    }

    Vec2 operator-(const Vec2& v)
    {
        return Vec2(x - v.x, y - v.y);
    }

    Vec2 operator*(const T& s)
    {
        return Vec2(x * s, y * s);
    }

    Vec2 operator/(const T& s)
    {
        return Vec2(x / s, y / s);
    }
};

template<class T>
class Vec3
{
public:
    T x, y, z;

    Vec3() {}

    Vec3(const Vec3& v)
    {
        *this = v;
    }

    Vec3(const T& x, const T& y, const T& z) : x(x), y(y), z(z) {}

    const Vec3& operator=(const Vec3& v)
    {
        x = v.x;
        y = v.y;
        z = v.z;
        return *this;
    }

    Vec3 operator+(const Vec3& v) const
    {
        return Vec3(x + v.x, y + v.y, z + v.z);
    }

    Vec3 operator-(const Vec3& v) const
    {
        return Vec3(x - v.x, y - v.y, z - v.z);
    }

    Vec3 operator*(const T& s) const
    {
        return Vec3(x * s, y * s, z * s);
    }

    Vec3 operator/(const T& s) const
    {
        return Vec3(x / s, y / s, z / s);
    }

    T dot(const Vec3& v) const
    {
        return v.x * x + v.y * y + v.z * z;
    }

    void normalize()
    {
        *this = *this / sqrt(this->dot(*this));
    }
};

typedef Vec2<int> Vec2i;
typedef Vec2<double> Vec2d;
typedef Vec3<double> Vec3d;
