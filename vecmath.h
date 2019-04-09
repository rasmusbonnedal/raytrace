#pragma once

#include <algorithm>

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

    Vec3 cross(const Vec3& v) const
    {
        return Vec3(
            y * v.z - z * v.y,
            z * v.x - x * v.z,
            x * v.y - y * v.x);
    }

    void normalize()
    {
        *this = *this / sqrt(this->dot(*this));
    }
};

template<class T>
Vec3<T> vmin(const Vec3<T>& v1, const Vec3<T>& v2) {
    return Vec3<T>(std::min(v1.x, v2.x),
        std::min(v1.y, v2.y),
        std::min(v1.z, v2.z));
}

template<class T>
Vec3<T> vmax(const Vec3<T>& v1, const Vec3<T>& v2) {
    return Vec3<T>(std::max(v1.x, v2.x),
        std::max(v1.y, v2.y),
        std::max(v1.z, v2.z));
}

typedef Vec2<int> Vec2i;
typedef Vec2<double> Vec2d;
typedef Vec3<unsigned> Vec3u;
typedef Vec3<double> Vec3d;
