#include "RayTracer.h"
#include "Utilities/Utilities.h"
#include "Framework/SceneManager.h"

#include <imgui.h>
#include <stb_image.h>
#include <tinyexr.h>

RayTracer::RayTracer(unsigned int screenWidth, unsigned int screenHeight, Scene* scene) : scene(scene)
{
	camera = new Camera(screenWidth, screenHeight);

	if(useSkydomeTexture)
	{
		const char* err = nullptr;
		int result = LoadEXR(&image, &width, &height, "Assets/studio.exr", &err);

		if(result != TINYEXR_SUCCESS)
		{
			fprintf(stderr, "ERR : %s\n", err);
			FreeEXRErrorMessage(err);
		}

		comp = sizeof(float);
	}
}

bool RayTracer::Update(float deltaTime)
{
	bool updated = false;
	ImGui::Begin("Skybox");
	if(ImGui::ColorEdit3("Sky A", &skyColorA.x)) { updated = true; }
	if(ImGui::ColorEdit3("Sky B", &skyColorB.x)) { updated = true; }
	if(ImGui::DragFloat("Sky Emission", &skydomeStrength, 0.01f)) { updated = true; }
	if(ImGui::DragFloat("Sky Offset", &skyDomeOffset, 0.01f, 0.0f, 1.0f)) { updated = true; }
	ImGui::End();

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

	outputColor.x = max(min(outputColor.x, 1.0f), 0.0);
	outputColor.y = max(min(outputColor.y, 1.0f), 0.0);
	outputColor.z = max(min(outputColor.z, 1.0f), 0.0);

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
		const Material& material = record.Primitive->Material;

		if(material.isEmissive)
		{
			// Emissive materials don't receive shading or bounce
			// They are considered to be lights.
			return material.Color * material.EmissiveStrength;
		}

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
		}
		else
		{
			vec3 bounceDir = RandomUnitVector();
			if(Dot(bounceDir, record.Normal) < 0.0f)
			{
				bounceDir = bounceDir * -1.0f;
			}

			Ray bounceRay = Ray(record.HitPoint, bounceDir);

			vec3 BRDF = vec3(0.0f);
			float diff = 0.0f;
			float spec = 0.0f;

			// Is mirror
			if(material.Specularity >= 0.999f)
			{
				diff = 0.0f;
				spec = 1.0f;
			}
			else // Exhibits diffuse & specular behaviour, include fresnel
			{
				float fresnel = Fresnel(ray.Direction, record.Normal, material.IoR);
				spec = min(material.Specularity + fresnel, 1.0f);
				diff = 1.0f - spec;
			}

			if(diff > 0.0f)
			{
				BRDF += diff * (material.Color * INVPI);
			}

			if(spec > 0.0f)
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

				if(material.Metalness > 0.0f)
				{
					vec3 radiance = TraverseScene(reflectRay, depth, record);
					BRDF += radiance * material.Color * material.Metalness;
					BRDF += radiance * (1.0f - material.Metalness);
				}
				else
				{
					BRDF += spec * TraverseScene(reflectRay, depth, record);
				}
			}

			float cosI = Dot(record.Normal, bounceDir);
			vec3 irradiance = TraverseScene(bounceRay, depth, record) * cosI;
			illumination += PI * 2.0f * BRDF * irradiance;
		}
	}
	else
	{
		if(depth == maxRayDepth - 1)
		{
			illumination += GetSkyColor(ray);
		}
		else
		{
			illumination += GetSkyColor(ray) * skydomeStrength;
		}
	}

	return illumination;
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

vec3 RayTracer::GetSkyColor(const Ray& ray)
{
	if(useSkydomeTexture)
	{
		float theta = acosf(ray.Direction.y);
		float phi = atan2f(ray.Direction.z, ray.Direction.x) + PI;

		float u = phi / (2 * PI);
		float v = theta / PI;

		u -= skyDomeOffset;
		
		if(u > 1.0f)
		{
			u -= 1.0f;
		}

		if(u < 0.0f)
		{
			u += 1.0f;
		}

		int i = (int)(u * width);
		int j = (int)(v * height);

		i = i % width;
		j = j % height;

		int index = (i + j * width) * comp;
		return vec3(&image[index]) * image[index + 3];
	}
	else
	{
		float t = max(ray.Direction.y, 0.0);
		return (1.0f - t) * skyColorA + skyColorB * t;
	}
}