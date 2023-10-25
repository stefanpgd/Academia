#pragma once
#include <vector>
#include <string>

#include "Primitive.h"
#include "Math/MathCommon.h"
#include "Camera.h"

struct Scene;

class RayTracer
{
public:
	RayTracer(unsigned int screenWidth, unsigned int screenHeight, Scene* scene);

	bool Update(float deltaTime);
	vec3 Trace(int pixelX, int pixelY);
	Primitive* SelectObject(int pixelX, int pixelY);
	
	void LoadSkydome();
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
	int maxRayDepth = 5;
	float maxLuminance = 50.0f;

	bool useSkydomeTexture = true;
	vec3 skyColorA = vec3(0.0f);
	vec3 skyColorB = vec3(0.84f, 0.72f, 1.0f);

	// Skydome //
	std::string skydomePath = "Assets/EXRs/studio.exr";
	int width, height, comp;
	float* image;

	friend class Editor;
};