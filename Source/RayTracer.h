#pragma once
#include <vector>
#include "Primitive.h"
#include "Math/MathCommon.h"

class RayTracer
{
public:
	RayTracer(unsigned int screenWidth, unsigned int screenHeight);

	unsigned int Trace(float xScale, float yScale);

private:
	void IntersectScene(const Ray& ray, HitRecord& record);

	vec3 DirectIllumination(const HitRecord& record);
	void IndirectIllumination();

private:
	// Camera //
	vec3 camera;
	vec3 viewDirection;
	
	// Screen virtual plane //
	vec3 screenCenter;
	vec3 screenP0, screenP1, screenP2;
	vec3 screenU, screenV;

	std::vector<Primitive*> scene;

	// Tracing settings // 
	float maxDepth = 1000.0f;
};