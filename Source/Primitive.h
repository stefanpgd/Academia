#pragma once
#include "Math/MathCommon.h"

class Ray;
class Primitive;

struct Material
{
	vec3 Color = vec3(1.0f);
	float Specularity = 0.0f;
	float Translucency = 0.0f;
	bool isEmissive = false;
};

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
	Material material;
};