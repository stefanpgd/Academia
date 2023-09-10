#pragma once
#include "Primitive.h"
#include "Math/MathCommon.h"

class Sphere : public Primitive
{
public:
	Sphere(vec3 position, float radius);

	virtual float Intersect(const Ray& ray) override;

	vec3 Position;
	float Radius;
	float Radius2;
};