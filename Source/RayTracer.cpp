#include "RayTracer.h"

// Primitives //
#include "Sphere.h"
#include "Plane.h"
#include "PlaneInfinite.h"
#include "Triangle.h"

#include "Utilities/Utilities.h"

#define Cornell true

RayTracer::RayTracer(unsigned int screenWidth, unsigned int screenHeight)
{
	// LH system
	camera = Vec3(0.5, 0.5f, -1.1f);
	viewDirection = Vec3(0.0f, 0.0f, 1.f);

	screenCenter = camera + viewDirection;

	screenP0 = screenCenter + vec3(-0.5f, -0.5f, 0.0f);		// Bottom Left
	screenP1 = screenCenter + vec3(0.5, -0.5, 0.0f);		// Bottom Right
	screenP2 = screenCenter + vec3(-0.5f, 0.5f, 0.0f);		// Top left

	screenU = screenP1 - screenP0;
	screenV = screenP2 - screenP0;

#if Cornell
	Plane* bottom = new Plane(vec3(0.0f, 0.0f, 0.0f), vec3(1.0f, 0.0f, 0.0f), vec3(0.0f, 0.0f, 1.0f));
	Plane* left = new Plane(vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 0.0f, 1.0f), vec3(0.0f, 1.0f, 0.0f));
	Plane* right = new Plane(vec3(1.0f, 0.0f, 1.0f), vec3(1.0f, 0.0f, 0.0f), vec3(1.0f, 1.0f, 1.0f));
	Plane* back = new Plane(vec3(0.0f, 0.0f, 1.0f), vec3(1.0f, 0.0f, 1.0f), vec3(0.0f, 1.0f, 1.0f));
	Plane* top = new Plane(vec3(0.0f, 1.0f, 1.0f), vec3(1.0f, 1.0f, 1.0f), vec3(0.0f, 1.0f, 0.0f));

	vec3 white = vec3(0.83f);
	vec3 red = vec3(0.65f, 0.05f, 0.05f);
	vec3 green = vec3(0.12f, 0.45f, 0.15f);

	left->material.Color = red;
	right->material.Color = green;
	bottom->material.Color = white;
	back->material.Color = white;
	top->material.Color = white;

	Plane* light = new Plane(vec3(0.4f, 0.999f, 0.6f), vec3(0.6f, 0.999f, 0.6f), vec3(0.4f, 0.999f, 0.4f));
	light->material.isEmissive = true;

	Sphere* glass = new Sphere(vec3(0.5, 0.5f, -0.75f), 0.05f);
	glass->material.isDielectric = true;

	Sphere* glass2 = new Sphere(vec3(0.88, 0.1f, 0.3f), 0.1f);
	glass2->material.isDielectric = true;

	Sphere* glass3 = new Sphere(vec3(0.12, 0.1f, 0.7f), 0.1f);
	glass3->material.isDielectric = true;

	Sphere* gloss = new Sphere(vec3(0.7, 0.1, 0.2), 0.1f);

	Plane* glassTest = new Plane(vec3(0.3f, 0.3f, 0.07f), vec3(0.7f, 0.3f, 0.1f), vec3(0.3f, 0.7f, 0.2f));
	glassTest->material.isDielectric = true;

	Sphere* gs = new Sphere(vec3(0.3f, 0.5f, 0.5f), 0.15f);
	//gs->material.isDielectric = true;

	scene.push_back(bottom);
	scene.push_back(left);
	scene.push_back(right);
	scene.push_back(back);
	scene.push_back(top);
	scene.push_back(light);
	scene.push_back(glass);
	scene.push_back(glass2);
	scene.push_back(glass3);
	//scene.push_back(gs);
	//scene.push_back(glassTest);

#else
	Plane* plane = new Plane(vec3(0.0f, 0.0f, 2.0f), vec3(0.5f, 0.0f, 2.5f), vec3(0.0f, 1.f, 2.0f));
	plane->material.Specularity = 0.99f;
	plane->material.Color = vec3(0.98f, 1.0f, 0.97f);

	Plane* plane2 = new Plane(vec3(1.5f, 0.0f, 2.5f), vec3(2.0f, 0.0f, 2.0f), vec3(1.5f, 1.f, 2.5f));
	plane2->material.Specularity = 0.8f;
	//plane2->material.Color = vec3(0.0f, 0.439, 0.231);

	Sphere* sphere = new Sphere(vec3(1.25f, 0.15f, 2.2f), 0.15f);
	sphere->material.Color = vec3(1.0f, 0.561f, 0.0f);

	Sphere* sphere2 = new Sphere(vec3(0.85f, 0.125f, 2.0f), 0.125f);
	sphere2->material.Specularity = 1.0f;

	PlaneInfinite* planeInf = new PlaneInfinite(vec3(0.0f, 0.0f, 5.0f), Normalize(vec3(0.0f, 1.0f, 0.0f)));

	scene.push_back(planeInf);
	scene.push_back(plane);
	scene.push_back(plane2);
	scene.push_back(sphere);
	scene.push_back(sphere2);
#endif
}

unsigned int RayTracer::Trace(float xScale, float yScale)
{
	vec3 outputColor = vec3(0.0f);

	vec3 screenPoint = screenP0 + (screenU * xScale) + (screenV * yScale);
	vec3 rayDirection = Normalize(screenPoint - camera);

	Ray ray = Ray(camera, rayDirection);

	HitRecord record;
	record.t = maxT;

	// Primary Ray //
	IntersectScene(ray, record);

	if (record.t != maxT)
	{
		if (record.Primitive->material.isEmissive)
		{
			vec3 c = record.Primitive->material.Color;
			return AlbedoToRGB(c.x, c.y, c.z);
		}

		float diffuse = 1.0f - record.Primitive->material.Specularity;
		float specular = record.Primitive->material.Specularity;

		if (record.Primitive->material.isDielectric)
		{
			outputColor += RefractionIllumination(record, ray, 1);
		}
		else
		{
			if (diffuse > 0.0f)
			{
				outputColor += diffuse * DirectIllumination(record);
			}

			if (specular > 0.0f)
			{
				outputColor += specular * IndirectIllumination(record, ray, 1);
			}
		}
	}
	else
	{
		// sky
		outputColor = GetSkyColor(ray);
	}

	// Do gamma correction?
	return AlbedoToRGB(outputColor.x, outputColor.y, outputColor.z);
}

void RayTracer::IntersectScene(const Ray& ray, HitRecord& record)
{
	HitRecord tempRecord;

	for (Primitive* primitive : scene)
	{
		primitive->Intersect(ray, tempRecord);

		if (tempRecord.t > EPSILON && tempRecord.t < record.t)
		{
			record = tempRecord;
		}
	}
}

vec3 RayTracer::DirectIllumination(const HitRecord& record)
{
	vec3 lightPos = vec3(0.5f, 0.93f, 0.5f); // hardcoded for now
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

	if (shadowRecord.t < r)
	{
		// Direct light is blocked by another surface
		return vec3(0.0f);
	}

	float diff = max(Dot(record.Normal, lightDir), 0.0);
	vec3 lightC = lightColor * min((lightIntensity / r2), 1.0f);

	return record.Primitive->material.Color * lightC * diff;
}

vec3 RayTracer::IndirectIllumination(const HitRecord& record, const Ray& ray, int rayDepth)
{
	if (rayDepth >= maxRayDepth)
	{
		// Max recursion depth exceeded
		return vec3(0.0f);
	}

	int currentRayDepth = rayDepth + 1;
	vec3 materialColor = record.Primitive->material.Color;

	vec3 reflected = Reflect(ray.Direction, Normalize(record.Normal));
	Ray reflectedRay = Ray(record.HitPoint, reflected);

	HitRecord reflectRecord;
	reflectRecord.t = maxT;

	IntersectScene(reflectedRay, reflectRecord);

	if (reflectRecord.t < maxT)
	{
		vec3 illumination(0.0f);

		float diffuse = 1.0f - reflectRecord.Primitive->material.Specularity;
		float specular = reflectRecord.Primitive->material.Specularity;

		if (diffuse > 0.0f)
		{
			illumination += diffuse * materialColor * DirectIllumination(reflectRecord);
		}

		if (specular > 0.0f)
		{
			illumination += specular * materialColor * IndirectIllumination(reflectRecord, ray, currentRayDepth);
		}

		// for now we don't step further, do in a minute
		return illumination;
	}

	// Return skybox
	return materialColor * GetSkyColor(reflectedRay);
}

vec3 RayTracer::GetSkyColor(const Ray& ray)
{
#if Cornell
	return vec3(0.0f);
#endif
	vec3 a = vec3(0.8f, 0.761f, 0.6f);
	vec3 b = vec3(0.475f, 0.91f, 1.0f);

	float t = max(ray.Direction.y, 0.0);

	return (1.0f - t) * a + b * t;
}

vec3 RayTracer::RefractionIllumination(const HitRecord& record, const Ray& ray, int rayDepth)
{
	if (rayDepth >= maxRayDepth)
	{
		// Max recursion depth exceeded
		return vec3(0.0f);
	}

	int currentRayDepth = rayDepth + 1;
	vec3 materialColor = record.Primitive->material.Color;

	vec3 rf = Refract(ray.Direction, record.Normal, 1.5f);
	Ray refractedRay = Ray(record.HitPoint, Normalize(rf));

	HitRecord refractRecord;
	refractRecord.t = maxT;

	IntersectScene(refractedRay, refractRecord);

	if (refractRecord.t < maxT)
	{
		vec3 illumination(0.0f);

		float diffuse = 1.0f - refractRecord.Primitive->material.Specularity;
		float specular = refractRecord.Primitive->material.Specularity;

		if (refractRecord.Primitive->material.isDielectric)
		{
			illumination += materialColor * RefractionIllumination(refractRecord, ray, currentRayDepth);
		}
		else
		{
			if (diffuse > 0.0f)
			{
				illumination += diffuse * materialColor * DirectIllumination(refractRecord);
			}

			if (specular > 0.0f)
			{
				illumination += specular * materialColor * IndirectIllumination(refractRecord, ray, currentRayDepth);
			}
		}

		// for now we don't step further, do in a minute
		return illumination;
	}

	// Return skybox
	return materialColor * GetSkyColor(refractedRay);
}
