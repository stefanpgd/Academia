#include "RayTracer.h"
#include "Utilities/Utilities.h"
#include "Framework/SceneManager.h"

#include <imgui.h>
#include <stb_image.h>
#include <tinyexr.h>

RayTracer::RayTracer(unsigned int screenWidth, unsigned int screenHeight, Scene* scene) : scene(scene)
{
	camera = scene->Camera;

	LoadSkydome();
}

bool RayTracer::Update(float deltaTime)
{
	bool updated = false;

	// Maybe in the future, let the scene manager own Camera and update it
	return camera->Update(deltaTime) || updated;
}

vec3 RayTracer::Trace(int pixelX, int pixelY)
{
	vec3 outputColor = vec3(0.0f);

	Ray ray = camera->GetRay(pixelX, pixelY);
	HitRecord record;

	// Randomly sample a single light from the scene every frame for a pixel //
	outputColor = TraverseScene(ray, maxRayDepth, record);

	outputColor.x = Clamp(outputColor.x, 0.0f, maxLuminance);
	outputColor.y = Clamp(outputColor.y, 0.0f, maxLuminance);
	outputColor.z = Clamp(outputColor.z, 0.0f, maxLuminance);

	return outputColor;
}

Primitive* RayTracer::SelectObject(int pixelX, int pixelY)
{
	Ray ray = camera->GetRay(pixelX, pixelY);
	HitRecord record;
	record.t = maxT;

	IntersectScene(ray, record);
	return record.Primitive;
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

	if(record.t >= maxT)
	{
		if(depth == maxRayDepth - 1)
		{
			illumination += GetSkyColor(ray) * scene->SkyDomeBackgroundStrength;
		}
		else
		{
			illumination += GetSkyColor(ray) * scene->SkyDomeEmission;
		}

		return illumination;
	}

	const Material& material = record.Primitive->Material;

	// Emissive Material Model //
	if(material.isEmissive)
	{
		// Emissive materials don't receive shading or bounce
		// They are considered to be lights.
		return material.Color * material.EmissiveStrength;
	}

	// Dielectric Material Model //
	if(material.isDielectric)
	{
		float reflectance = Fresnel(ray.Direction, record.Normal, material.IoR);
		float transmittance = 1.0f - reflectance;

		// Reflection //
		Ray reflectedRay = Ray(record.HitPoint, Reflect(ray.Direction, record.Normal));
		illumination += TraverseScene(reflectedRay, depth, record) * reflectance;

		// Refraction // 
		vec3 Rt = Refract(ray.Direction, record.Normal, material.IoR);
		Ray refractedRay = Ray(record.HitPoint + Rt * EPSILONSMALL, Rt);

		vec3 c = material.Color;
		if(record.InsideMedium)
		{
			float transmittedDistance = (record.HitPoint - lastRecord.HitPoint).Magnitude();
			float beer = expf(-material.Density * transmittedDistance);
			c = c * beer;
		}

		illumination += c * TraverseScene(refractedRay, depth, record) * transmittance;

		return illumination;
	}

	// Opaque Material Model //

	// Russian Roulette (Variance Reduction)
	float multiplier = 1.0f;
	float brightestChannel = max(max(material.Color.x, material.Color.y), material.Color.z);
	float survivalRate = Clamp(brightestChannel, 0.1f, 0.9f);

	if(survivalRate < Random01())
	{
		return vec3(0.0f);
	}
	multiplier = 1.0f / survivalRate;

	float fresnel = Fresnel(ray.Direction, record.Normal, material.IoR);
	float specularity = min(material.Specularity + fresnel, 1.0f);
	float diffuse = 1.0f - specularity;

	if(diffuse > 0.0f)
	{
		vec3 bounceDir = RandomUnitVector();
		if(Dot(bounceDir, record.Normal) < 0.0f)
		{
			bounceDir = bounceDir * -1.0f;
		}

		Ray bounceRay = Ray(record.HitPoint, bounceDir);

		vec3 BRDF = diffuse * (material.Color * INVPI);
		vec3 radiance = TraverseScene(bounceRay, depth, record);
		float cosI = Dot(record.Normal, bounceDir);
		const float area = PI * 2.0f;

		// Hemispherical rendering equation // 
		illumination += area * BRDF * radiance * cosI;
	}

	if(specularity > 0.0f)
	{
		Ray reflectRay;

		if(material.Roughness > 0.0f)
		{
			vec3 offset = RandomUnitVector() * material.Roughness;
			reflectRay = Ray(record.HitPoint, Reflect(Normalize(ray.Direction + offset), record.Normal));
		}
		else
		{
			reflectRay = Ray(record.HitPoint, Reflect(ray.Direction, record.Normal));
		}

		vec3 radiance = TraverseScene(reflectRay, depth, record);
		if(material.Metalness > 0.0f)
		{
			float metallicness = material.Metalness * specularity;
			float specular = (1.0f - material.Metalness) * specularity;

			illumination += metallicness * material.Color * radiance;
			illumination += specular * radiance;
		}
		else
		{
			illumination += specularity * radiance;
		}
	}

	return illumination * multiplier;
}

void RayTracer::IntersectScene(const Ray& ray, HitRecord& record)
{
	HitRecord tempRecord;
	tempRecord.InsideMedium = record.InsideMedium;

	for(Primitive* primitive : scene->primitives)
	{
		primitive->Intersect(ray, tempRecord);

		if(tempRecord.t > EPSILON && tempRecord.t < record.t)
		{
			record = tempRecord;
		}
	}
}

void RayTracer::LoadSkydome()
{
	delete image;

	const char* err = nullptr;
	int result = LoadEXR(&image, &width, &height, skydomePath.c_str(), &err);
	comp = sizeof(float);

	if(result != TINYEXR_SUCCESS)
	{
		fprintf(stderr, "ERR : %s\n", err);
		FreeEXRErrorMessage(err);
	}
}

vec3 RayTracer::GetSkyColor(const Ray& ray)
{
	if(useSkydomeTexture)
	{
		float theta = acosf(ray.Direction.y);
		float phi = atan2f(ray.Direction.z, ray.Direction.x) + PI;

		float u = phi / (2 * PI);
		float v = theta / PI;

		u -= scene->SkydomeOrientation;

		int i = (int)((1.0f - u) * width);
		int j = (int)(v * height);

		i = i % width;
		j = j % height;

		int index = (i + j * width) * comp;
		vec3 b = vec3(image[index], image[index + 1], image[index + 2]);
		return b;
	}
	else
	{
		float t = max(ray.Direction.y, 0.0);
		return (1.0f - t) * skyColorA + skyColorB * t;
	}
}