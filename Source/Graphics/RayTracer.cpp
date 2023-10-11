#include "RayTracer.h"
#include "Utilities/Utilities.h"
#include "Framework/SceneManager.h"

#include <imgui.h>

#define ior 1.5f

RayTracer::RayTracer(unsigned int screenWidth, unsigned int screenHeight, Scene* scene) : scene(scene)
{
	camera = new Camera(screenWidth, screenHeight);
}

bool RayTracer::Update(float deltaTime)
{
	bool updated = false;
	ImGui::Begin("Skybox");
	if(ImGui::ColorEdit3("Sky A", &skyColorA.x)) { updated = true; }
	if(ImGui::ColorEdit3("Sky B", &skyColorB.x)) { updated = true; }
	ImGui::End();

	// Maybe in the future, let the scene manager own Camera and update it
	return camera->Update(deltaTime) || updated;
}

vec3 RayTracer::Trace(int pixelX, int pixelY, int currentDepth)
{
	vec3 outputColor = vec3(0.0f);
	
	Ray ray = camera->GetRay(pixelX, pixelY);
	HitRecord record;

	// Randomly sample a single light from the scene every frame for a pixel //
	outputColor = TraverseScene(ray, currentDepth, record);

	return outputColor;
}

void RayTracer::Resize(int width, int height)
{
	camera->SetupVirtualPlane(width, height);
}

vec3 RayTracer::TraverseScene(const Ray& ray, int rayDepth, const HitRecord& lastRecord)
{
	if(rayDepth <= 0)
	{
		// Exceeded bounce limit
		return vec3(0.0f);
	}

	int depth = rayDepth - 1;

	HitRecord record;
	record.t = maxT;
	record.InsideMedium = lastRecord.InsideMedium;

	IntersectScene(ray, record);

	vec3 illumination = vec3(0.0f);

	if(record.t < maxT)
	{
		vec3 materialColor = record.Primitive->material.Color;

		if(record.Primitive->material.isEmissive)
		{
			// Emissive materials don't receive shading or bounce
			// They are considered to be lights.
			return record.Primitive->material.Color;
		}

		vec3 bounceDir = RandomUnitVector();
		if(Dot(bounceDir, record.Normal) < 0.0f)
		{
			bounceDir = bounceDir * -1.0f;
		}

		Ray bounceRay = Ray(record.HitPoint, bounceDir);
		vec3 BRDF = materialColor * INVPI;
		float cosI = Dot(record.Normal, bounceDir);
		vec3 irradiance = TraverseScene(bounceRay, depth, record) * cosI;

		illumination += PI * 2.0f * BRDF * irradiance;
	}
	else
	{
		illumination += GetSkyColor(ray);
	}

	illumination.x = min(illumination.x, 1.0f);
	illumination.y = min(illumination.y, 1.0f);
	illumination.z = min(illumination.z, 1.0f);

	return illumination;
}

void RayTracer::IntersectScene(const Ray& ray, HitRecord& record)
{
	HitRecord tempRecord;
	tempRecord.InsideMedium = record.InsideMedium;

	for (Primitive* primitive : scene->primitives)
	{
		primitive->Intersect(ray, tempRecord);

		if (tempRecord.t > EPSILON && tempRecord.t < record.t)
		{
			record = tempRecord;
		}
	}
}

vec3 RayTracer::GetSkyColor(const Ray& ray)
{
	//return vec3(0.0f);

	float t = max(ray.Direction.y, 0.0);
	return (1.0f - t) * skyColorA + skyColorB * t;
}