#include "RayTracer.h"

// Primitives //
#include "Sphere.h"
#include "Plane.h"
#include "PlaneInfinite.h"
#include "Triangle.h"

#include "Utilities/Utilities.h"

#define Cornell true
#define ior 1.5f

RayTracer::RayTracer(unsigned int screenWidth, unsigned int screenHeight) 
{
	camera = new Camera(screenWidth, screenHeight);

#if Cornell
	Plane* bottom = new Plane(vec3(0.0f, 0.0f, 0.0f), vec3(1.0f, 0.0f, 0.0f), vec3(0.0f, 0.0f, 1.0f));
	Plane* left = new Plane(vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 0.0f, 1.0f), vec3(0.0f, 1.0f, 0.0f));
	Plane* right = new Plane(vec3(1.0f, 0.0f, 1.0f), vec3(1.0f, 0.0f, 0.0f), vec3(1.0f, 1.0f, 1.0f));
	Plane* back = new Plane(vec3(0.0f, 0.0f, 1.0f), vec3(1.0f, 0.0f, 1.0f), vec3(0.0f, 1.0f, 1.0f));
	Plane* top = new Plane(vec3(0.0f, 1.0f, 1.0f), vec3(1.0f, 1.0f, 1.0f), vec3(0.0f, 1.0f, 0.0f));

	vec3 white = vec3(0.83f);
	vec3 orange = vec3(1, 0.578, 0.067);
	vec3 blue = vec3(0.067, 0.698, 1);

	left->material.Color = orange;
	right->material.Color = blue;
	bottom->material.Color = white;
	back->material.Color = white;
	top->material.Color = white;

	Plane* light = new Plane(vec3(0.35f, 0.999f, 0.65f), vec3(0.65f, 0.999f, 0.65f), vec3(0.35f, 0.999f, 0.35f));
	light->material.isEmissive = true;

	Sphere* metal = new Sphere(vec3(0.3, 0.15f, 0.7), 0.15f);
	metal->material.Color = vec3(0.95f, 0.92f, 0.96f);
	metal->material.Specularity = 1.0f;
	metal->material.SpecularGloss = 0.98f;

	Sphere* glass2 = new Sphere(vec3(0.75, 0.165f, 0.35f), 0.165f);
	glass2->material.isDielectric = true;
	glass2->material.SpecularGloss = 0.8f;

	scene.push_back(bottom);
	scene.push_back(left);
	scene.push_back(right);
	scene.push_back(back);
	scene.push_back(top);
	scene.push_back(light);
	scene.push_back(glass2);
	scene.push_back(metal);

#else
	Sphere* sphere = new Sphere(vec3(0.5f, 0.5f, 0.5f), 0.25f);
	sphere->material.Color = vec3(1.0f, 1.0f, 1.0f);
	sphere->material.isDielectric = true;

	//Sphere* sphere2 = new Sphere(vec3(0.5f, 0.5f, 6.5f), 0.25f);
	//sphere2->material.Color = vec3(1.0f, 1.0f, 1.0f);
	//sphere2->material.isDielectric = true;

	PlaneInfinite* plane = new PlaneInfinite(vec3(0.0f, 0.25f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
	plane->material.Specularity = 0.1f;

	scene.push_back(sphere);
	//scene.push_back(sphere2);
	//scene.push_back(plane);
#endif
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

	outputColor = TraverseScene(ray, currentDepth, record);

	return outputColor;
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

	for (Primitive* primitive : scene)
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
	vec3 lightPos = vec3(0.5f, 0.93f, 0.5f); // hardcoded for now
	lightPos.x += RandomInRange(-0.075f, 0.075f);
	lightPos.y += RandomInRange(-0.075f, 0.075f);

	vec3 lightColor = vec3(1.0f);
	vec3 lightDir = lightPos - record.HitPoint;
	float lightIntensity = 0.5f;

	float r = lightDir.Magnitude();
	float r2 = r * r;

	lightDir.Normalize();

	Ray shadowRay = Ray(record.HitPoint, lightDir);

	HitRecord shadowRecord;
	shadowRecord.t = r;

	IntersectScene(shadowRay, shadowRecord);

	if(shadowRecord.t < r)
	{
		// Direct light is blocked by another surface
		return vec3(0.0f);
	}

	float diff = max(Dot(record.Normal, lightDir), 0.0);
	vec3 lightC = lightColor * min((lightIntensity / r2), 1.0f);

	return record.Primitive->material.Color * lightC * diff;
}

vec3 RayTracer::GetSkyColor(const Ray& ray)
{
#if Cornell
	return vec3(0.0f);
#endif
	vec3 a = vec3(1, 0.578, 0.067);
	vec3 b = vec3(0.475f, 0.91f, 1.0f);

	float t = max(ray.Direction.y, 0.0);

	return (1.0f - t) * a + b * t;
}