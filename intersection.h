#pragma once

#include "vecmath.h"

class Intersection
{
public:
    Intersection() {}

    Intersection(Vec3d intersection, Vec3d normal)
        : intersection(intersection), normal(normal)
    {
    }

    Vec3d intersection;
    Vec3d normal;
};