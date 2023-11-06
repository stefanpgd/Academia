#pragma once
#include <vector>
#include <string>

#include "Primitive.h"
#include "Math/MathCommon.h"
#include "Camera.h"

struct Scene;
struct Skydome;

class RayTracer
{
public:
	RayTracer(unsigned int screenWidth, unsigned int screenHeight, Scene* scene);

	vec3 Trace(int pixelX, int pixelY);
	Primitive* SelectObject(int pixelX, int pixelY);
	
private:
	vec3 TraverseScene(const Ray& ray, int rayDepth, const HitRecord& lastRecord);
	void IntersectScene(const Ray& ray, HitRecord& record);

	vec3 GetSkyColor(const Ray& ray);

private:
	Scene* scene;
	Skydome* skydome;
	Camera* camera;

	// Tracing settings // 
	float maxT = 100.0f;
	int maxRayDepth = 16;
	float maxLuminance = 50.0f;

	bool useSkydomeTexture = true;
	vec3 skyColorA = vec3(0.0f);
	vec3 skyColorB = vec3(0.84f, 0.72f, 1.0f);

	friend class Editor;
};