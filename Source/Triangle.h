#pragma once
#include "Primitive.h"

class Triangle : public Primitive
{
public:
	Triangle(vec3 v0, vec3 v1, vec3 v2);
	virtual void Intersect(const Ray& ray, HitRecord& record) override;

private:
	vec3 Normal;
	vec3 v0, v1, v2;
	vec3 e1, e2;
};