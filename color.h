#pragma once

#include "vecmath.h"

#include <algorithm>

class Color8
{
public:
    Color8(unsigned char r, unsigned char g, unsigned char b) : r(r), g(g), b(b)
    {
    }

    unsigned char r, g, b;
};

class Color
{
public:
    Color() {}

    Color(const Vec3d& v) : r(v.x), g(v.y), b(v.z) {}

    Color8 clamp() const { return Color8(clamp(r), clamp(g), clamp(b)); }

    double r, g, b;

private:
    static unsigned char clamp(double v)
    {
        return std::max(0, std::min(255, static_cast<int>(v * 255.0)));
    }
};
