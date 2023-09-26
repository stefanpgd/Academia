#pragma once
#include "Primitive.h"

class Plane : public Primitive
{
public:
	Plane(vec3 v0, vec3 v1, vec3 v2);

	vec3 Normal;

private:
	virtual void Intersect(const Ray& ray, HitRecord& record) override;

	vec3 u, v, v0;
	float w, h;
};