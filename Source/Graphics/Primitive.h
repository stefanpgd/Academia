#pragma once
#include "Math/MathCommon.h"

class Ray;
class Primitive;

// Material models?
// 1. Opaque
// 2. Dielectric?
// 3. Emissive

struct Material
{
	vec3 Color = vec3(1.0f);
	float Specularity = 0.0f;
	float Roughness = 0.0f;
	float Metalness = 0.0f;
	float IoR = 1.0f;

	float EmissiveStrength = 1.0f;
	bool isEmissive = false;

	bool isDielectric = false;
};

struct Light
{
	vec3 Color = vec3(1.0f);
	vec3 Position = vec3(0.0f);
	float Intensity = 1.0f;

	bool HasArea = false;
	vec3 Scale = vec3(0.0f);
};

struct HitRecord
{
	float t;
	vec3 HitPoint;
	vec3 Normal;
	Primitive* Primitive;
	bool InsideMedium = false;
};


enum class PrimitiveType
{
	Sphere,
	Plane,
	PlaneInfinite,
	Triangle 
};

class Primitive
{
public:
	virtual void Intersect(const Ray& ray, HitRecord& record) = 0;

	vec3 Position;
	Material Material;
	PrimitiveType Type;
};