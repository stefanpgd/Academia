#include "RayTracer.h"

// Primitives //
#include "Sphere.h"
#include "Plane.h"
#include "PlaneInfinite.h"
#include "Triangle.h"

#include "Utilities/Utilities.h"

#define Cornell true
#define ior 1.50f

RayTracer::RayTracer(unsigned int screenWidth, unsigned int screenHeight) : screenWidth(screenWidth), screenHeight(screenHeight)
{
	// LH system
	camera = Vec3(0.5, 0.5f, -1.0f);
	viewDirection = Vec3(0.0f, 0.0f, 1.f);

	screenCenter = camera + viewDirection;

	screenP0 = screenCenter + vec3(-0.5f, -0.5f, 0.0f);		// Bottom Left
	screenP1 = screenCenter + vec3(0.5, -0.5, 0.0f);		// Bottom Right
	screenP2 = screenCenter + vec3(-0.5f, 0.5f, 0.0f);		// Top left

	screenU = screenP1 - screenP0;
	screenV = screenP2 - screenP0;

	pixelSizeX = (1.0f / float(screenWidth)) * 0.5f;
	pixelSizeY = (1.0f / float(screenHeight)) * 0.5f;

#if Cornell
	Plane* bottom = new Plane(vec3(0.0f, 0.0f, 0.0f), vec3(1.0f, 0.0f, 0.0f), vec3(0.0f, 0.0f, 1.0f));
	Plane* left = new Plane(vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 0.0f, 1.0f), vec3(0.0f, 1.0f, 0.0f));
	Plane* right = new Plane(vec3(1.0f, 0.0f, 1.0f), vec3(1.0f, 0.0f, 0.0f), vec3(1.0f, 1.0f, 1.0f));
	Plane* back = new Plane(vec3(0.0f, 0.0f, 1.0f), vec3(1.0f, 0.0f, 1.0f), vec3(0.0f, 1.0f, 1.0f));
	Plane* top = new Plane(vec3(0.0f, 1.0f, 1.0f), vec3(1.0f, 1.0f, 1.0f), vec3(0.0f, 1.0f, 0.0f));

	vec3 white = vec3(0.83f);
	vec3 red = vec3(1, 0.578, 0.067);
	vec3 green = vec3(0.067, 0.698, 1);

	left->material.Color = red;
	right->material.Color = green;
	bottom->material.Color = white;
	back->material.Color = white;
	top->material.Color = white;

	Plane* light = new Plane(vec3(0.4f, 0.999f, 0.6f), vec3(0.6f, 0.999f, 0.6f), vec3(0.4f, 0.999f, 0.4f));
	light->material.isEmissive = true;

	Sphere* metal = new Sphere(vec3(0.3, 0.15f, 0.7), 0.15f);
	metal->material.Specularity = 1.0f;

	Sphere* glass2 = new Sphere(vec3(0.75, 0.165f, 0.35f), 0.165f);
	glass2->material.isDielectric = true;

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

vec3 RayTracer::Trace(int pixelX, int pixelY, int currentDepth)
{
	vec3 outputColor = vec3(0.0f);

	// determine where on the virtual screen we need to be //
	float xScale = pixelX / float(screenWidth);
	float yScale = pixelY / float(screenHeight);

	// Anti-Aliasing (Monte-Carlo? - Double check the lecture)
	xScale += RandomInRange(-pixelSizeX, pixelSizeX);
	yScale += RandomInRange(-pixelSizeY, pixelSizeY);

	vec3 screenPoint = screenP0 + (screenU * xScale) + (screenV * yScale);
	vec3 rayDirection = Normalize(screenPoint - camera);

	Ray ray = Ray(camera, rayDirection);
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
	lightPos.x += RandomInRange(-0.05f, 0.05f);
	lightPos.y += RandomInRange(-0.05f, 0.05f);

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