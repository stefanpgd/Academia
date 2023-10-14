#pragma once
#include <vector>
#include "Primitive.h"
#include "Math/MathCommon.h"
#include "Camera.h"

struct Scene;

class RayTracer
{
public:
	RayTracer(unsigned int screenWidth, unsigned int screenHeight, Scene* scene);

	bool Update(float deltaTime);
	vec3 Trace(int pixelX, int pixelY, int maxDepth);
	
	void Resize(int width, int height);

private:
	vec3 TraverseScene(const Ray& ray, int rayDepth, const HitRecord& lastRecord);
	void IntersectScene(const Ray& ray, HitRecord& record);

	vec3 GetSkyColor(const Ray& ray);

private:
	Scene* scene;
	Camera* camera;

	// Tracing settings // 
	float maxT = 100.0f;

	vec3 skyColorA = vec3(0.0f);
	vec3 skyColorB = vec3(0.84f, 0.72f, 1.0f);
};