#pragma once
#include "Math/MathCommon.h"

class Ray;
class Primitive;

struct HitRecord
{
	float t;
	vec3 HitPoint;
	vec3 Normal;
	Primitive* Primitive;
};

class Primitive
{
public:
	virtual void Intersect(const Ray& ray, HitRecord& record) = 0;

	vec3 Position;
	vec3 Color;
};