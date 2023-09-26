#pragma once
#include "Primitive.h"

class PlaneInfinite : public Primitive
{
public:
	PlaneInfinite(vec3 position, vec3 normal);

	virtual void Intersect(const Ray& ray, HitRecord& record) override;

	vec3 Normal;
};

