#include "RayTracer.h"
#include "Utilities/Utilities.h"
#include "Framework/SceneManager.h"

#define ior 1.5f

RayTracer::RayTracer(unsigned int screenWidth, unsigned int screenHeight, Scene* scene) : scene(scene)
{
	camera = new Camera(screenWidth, screenHeight);
}

bool RayTracer::Update(float deltaTime)
{
	// Maybe in the future, let the scene manager own Camera and update it
	return camera->Update(deltaTime);
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

		if(record.Primitive->material.isDielectric)
		{
			float reflectance = Fresnel(ray.Direction, record.Normal, ior);
			float transmittance = 1.0f - reflectance;

			if(reflectance > 0.0f)
			{
				vec3 reflected = Reflect(ray.Direction, Normalize(record.Normal));
				
				if(record.Primitive->material.SpecularGloss < 1.0f)
				{
					reflected += (1.0f - record.Primitive->material.SpecularGloss) * RandomUnitVector();
					reflected.Normalize();
				}

				Ray reflectedRay = Ray(record.HitPoint, reflected);

				illumination += reflectance * materialColor * TraverseScene(reflectedRay, depth, record);
			}

			if(transmittance > 0.0f)
			{
				vec3 rf = Refract(ray.Direction, record.Normal, ior);
				Ray refractRay = Ray(record.HitPoint + rf * EPSILON, rf);

				illumination += transmittance * materialColor * TraverseScene(refractRay, depth, record);
			}
		}
		else
		{
			float diffuse = 1.0f - record.Primitive->material.Specularity;
			float specular = record.Primitive->material.Specularity;

			if(diffuse > 0.0f)
			{
				illumination += diffuse * CalculateDiffuseShading(record);
			}

			if(specular > 0.0f)
			{
				vec3 reflected = Reflect(ray.Direction, Normalize(record.Normal));

				if(record.Primitive->material.SpecularGloss < 1.0f)
				{
					reflected += (1.0f - record.Primitive->material.SpecularGloss) * RandomUnitVector();
					reflected.Normalize();
				}

				Ray reflectedRay = Ray(record.HitPoint, reflected);

				illumination += specular * materialColor * TraverseScene(reflectedRay, depth, record);
			}
		}
	}
	else
	{
		illumination += GetSkyColor(ray);
	}

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

vec3 RayTracer::CalculateDiffuseShading(const HitRecord& record)
{
	vec3 accumaltedLight = vec3(0.0f);

	for(Light* light : scene->lights)
	{
		vec3 lightPosition = light->Position;

		if(light->HasArea)
		{
			vec3 offset;
			offset.x = RandomInRange(-light->Scale.x, light->Scale.x);
			offset.y = RandomInRange(-light->Scale.y, light->Scale.y);
			offset.z = RandomInRange(-light->Scale.z, light->Scale.z);

			lightPosition += offset;
		}
		vec3 lightDir = lightPosition - record.HitPoint;

		float r = lightDir.Magnitude();
		float r2 = r * r;

		lightDir.Normalize();

		Ray shadowRay = Ray(record.HitPoint, lightDir);

		HitRecord shadowRecord;
		shadowRecord.t = r;

		IntersectScene(shadowRay, shadowRecord);

		// No intersection between hitpoint & light
		if(shadowRecord.t == r)
		{
			float cosI = max(Dot(record.Normal, lightDir), 0.0);
			accumaltedLight += light->Color * min((light->Intensity / r2), 1.0f) * cosI;
		}
	}

	// TO DO: Add clamp util for vec3
	accumaltedLight.x = min(accumaltedLight.x, 1.0f);
	accumaltedLight.y = min(accumaltedLight.y, 1.0f);
	accumaltedLight.z = min(accumaltedLight.z, 1.0f);

	return record.Primitive->material.Color * accumaltedLight;
}

vec3 RayTracer::GetSkyColor(const Ray& ray)
{
	return vec3(0.0f);
	
	vec3 a = vec3(1, 0.578, 0.067);
	vec3 b = vec3(0.475f, 0.91f, 1.0f);

	float t = max(ray.Direction.y, 0.0);

	return (1.0f - t) * a + b * t;
}