#include "RayTracer.h"

// Primitives //
#include "Sphere.h"
#include "Plane.h"
#include "PlaneInfinite.h"
#include "Triangle.h"

#include "Utilities/Utilities.h"

RayTracer::RayTracer(unsigned int screenWidth, unsigned int screenHeight)
{
	// LH system
	camera = Vec3(0.5f, 0.5f, 0.0f);
	viewDirection = Vec3(0.0f, 0.0f, 1.0f);

	screenCenter = camera + viewDirection;

	screenP0 = screenCenter + vec3(-0.5f, -0.5f, 0.0f);	// Bottom Left
	screenP1 = screenCenter + vec3(0.5, -0.5, 0.0f);		// Bottom Right
	screenP2 = screenCenter + vec3(-0.5f, 0.5f, 0.0f);		// Top left

	float aspect = screenWidth / float(screenHeight);
	screenU = (screenP1 - screenP0) * aspect;
	screenV = screenP2 - screenP0;

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
		float diffuse = 1.0f - record.Primitive->material.Specularity;
		float specular = record.Primitive->material.Specularity;

		if (diffuse > 0.0f)
		{
			outputColor += diffuse * DirectIllumination(record);
		}

		if (specular > 0.0f)
		{
			outputColor += specular * IndirectIllumination(record, ray, 1);
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
	vec3 lightPos = vec3(1.35f, 2.15f, 2.0f); // hardcoded for now
	vec3 lightColor = vec3(0.9f, 0.7f, 1.0f);
	vec3 lightDir = lightPos - record.HitPoint;
	float lightIntensity = 1.75f;

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
	vec3 a = vec3(0.8f, 0.761f, 0.6f);
	vec3 b = vec3(0.475f, 0.91f, 1.0f);

	float t = max(ray.Direction.y, 0.0);

	return (1.0f - t) * a + b * t;
}
