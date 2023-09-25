#pragma once
#include <vector>
#include "Primitive.h"
#include "Math/MathCommon.h"
#include "Camera.h"

class RayTracer
{
public:
	RayTracer(unsigned int screenWidth, unsigned int screenHeight);

	bool Update(float deltaTime);
	vec3 Trace(int pixelX, int pixelY, int maxDepth);

private:
	vec3 TraverseScene(const Ray& ray, int rayDepth, const HitRecord& lastRecord);

	void IntersectScene(const Ray& ray, HitRecord& record);

	vec3 CalculateDiffuseShading(const HitRecord& record);
	vec3 GetSkyColor(const Ray& ray);

private:
	std::vector<Primitive*> scene;

	Camera* camera;

	// Tracing settings // 
	float maxT = 100.0f;
};