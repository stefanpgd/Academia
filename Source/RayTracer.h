#pragma once
#include <vector>
#include "Primitive.h"
#include "Math/MathCommon.h"

class RayTracer
{
public:
	RayTracer(unsigned int screenWidth, unsigned int screenHeight);

	vec3 Trace(int pixelX, int pixelY, int maxDepth);

private:
	vec3 TraverseScene(const Ray& ray, int rayDepth, const HitRecord& lastRecord);

	void IntersectScene(const Ray& ray, HitRecord& record);

	vec3 CalculateDiffuseShading(const HitRecord& record);
	vec3 GetSkyColor(const Ray& ray);

private:
	// Camera //
	vec3 camera;
	vec3 viewDirection;
	
	// Screen virtual plane //
	vec3 screenCenter;
	vec3 screenP0, screenP1, screenP2;
	vec3 screenU, screenV;
	unsigned int screenWidth, screenHeight;
	float pixelSizeX, pixelSizeY;

	std::vector<Primitive*> scene;

	// Tracing settings // 
	float maxT = 100.0f;
};