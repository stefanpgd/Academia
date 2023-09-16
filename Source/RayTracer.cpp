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
	vec3 screenPoint = screenP0 + (screenU * xScale) + (screenV * yScale);
	vec3 rayDirection = Normalize(screenPoint - camera);

	Ray ray = Ray(camera, rayDirection);

	HitRecord record;
	record.t = maxDepth;

	IntersectScene(ray, record);

	if (record.t != maxDepth)
	{
		return AlbedoToRGB(1.0f, 0.0f, 0.0f);
	}

	return 0x00;
}

void RayTracer::IntersectScene(const Ray& ray, HitRecord& record)
{
	HitRecord tempRecord;

	for (Primitive* primitive : scene)
	{
		primitive->Intersect(ray, tempRecord);

		if (tempRecord.t > 0.0f && tempRecord.t < record.t)
		{
			record = tempRecord;
		}
	}
}