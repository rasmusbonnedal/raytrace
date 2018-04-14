#pragma once

#include "vecmath.h"

class Ray
{
public:
    Vec3d origin, direction;

    Ray(const Vec3d& origin, const Vec3d& direction)
        : origin(origin), direction(direction)
    {
    }
};
