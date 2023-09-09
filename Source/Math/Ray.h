#pragma once

#include "Vec3.h"

struct Ray
{
public:
	Ray();
	Ray(const Vec3& origin, const Vec3& direction);

	Vec3 At(float t);
	
	Vec3 Origin;
	Vec3 Direction;
};

typedef Ray ray;