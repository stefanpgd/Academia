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
	camera = Vec3(0.0f, 0.5f, 0.0f);
	viewDirection = Vec3(0.0f, 0.0f, 1.0f);

	screenCenter = camera + viewDirection;

	screenP0 = screenCenter + vec3(-0.5f, -0.5f, 0.0f);	// Bottom Left
	screenP1 = screenCenter + vec3(0.5, -0.5, 0.0f);		// Bottom Right
	screenP2 = screenCenter + vec3(-0.5f, 0.5f, 0.0f);		// Top left

	float aspect = screenWidth / float(screenHeight);
	screenU = (screenP1 - screenP0) * aspect;
	screenV = screenP2 - screenP0;

	scene.push_back(new Sphere(vec3(2.5f, 0.15f, 4.0f), 0.15f, vec3(0.4f, 1.0f, 0.6f)));
	scene.push_back(new Plane(vec3(-0.5f, 0.0f, 4.0f), vec3(1.0f, 0.0f, 9.0f), vec3(-0.5f, 3.0f, 6.0f)));
	scene.push_back(new PlaneInfinite(vec3(0.0f, 0.0f, 5.0f), Normalize(vec3(0.0f, 1.0f, 0.0f))));
	scene.push_back(new Triangle(vec3(1.0f, 0.5f, 3.0f), vec3(2.0f, 0.5f, 3.0f), vec3(1.5f, 1.0f, 3.0f)));
}

unsigned int RayTracer::Trace(float xScale, float yScale)
{
	vec3 outputColor = vec3(0.0f);

	vec3 screenPoint = screenP0 + (screenU * xScale) + (screenV * yScale);
	vec3 rayDirection = Normalize(screenPoint - camera);

	Ray ray = Ray(camera, rayDirection);

	HitRecord record;
	record.t = maxDepth;

	// Primary Ray //
	IntersectScene(ray, record);

	if (record.t != maxDepth)
	{
		outputColor += record.Primitive->Color * DirectIllumination(record);
	}
	// else, skybox?

	return AlbedoToRGB(outputColor.x, outputColor.y, outputColor.z);
}

void RayTracer::IntersectScene(const Ray& ray, HitRecord& record)
{
	HitRecord tempRecord;

	for (Primitive* primitive : scene)
	{
		if (record.Primitive == primitive)
		{
			continue;
		}

		primitive->Intersect(ray, tempRecord);

		if (tempRecord.t > 0.0001f && tempRecord.t < record.t)
		{
			record = tempRecord;
		}
	}
}

vec3 RayTracer::DirectIllumination(const HitRecord& record)
{
	vec3 lightPos = vec3(2.0f, 10.0f, 0.0f); // hardcoded for now
	vec3 lightColor = vec3(1.0f);

	vec3 lightDir = Normalize(lightPos - record.HitPoint);

	Ray shadowRay = Ray(record.HitPoint, lightDir);

	HitRecord shadowRecord;
	shadowRecord.t = maxDepth;

	IntersectScene(shadowRay, shadowRecord);

	if (shadowRecord.t < maxDepth)
	{
		// Direct light is blocked by another surface
		return vec3(0.0f);
	}

	float diff = max(Dot(record.Normal, lightDir), 0.0);
	return lightColor * diff;
}