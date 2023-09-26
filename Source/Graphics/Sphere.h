#pragma once
#include "Primitive.h"

class Sphere : public Primitive
{
public:
	Sphere(vec3 position, float radius);
	Sphere(vec3 position, float radius, vec3 color);

	virtual void Intersect(const Ray& ray, HitRecord& record) override;

	float Radius;
	float Radius2;
};