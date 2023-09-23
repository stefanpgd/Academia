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
	// LH system
	camera = Vec3(0.5, 0.5f, -1.0f);
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
	vec3 red = vec3(1, 0.578, 0.067);
	vec3 green = vec3(0.067, 0.698, 1);

	left->material.Color = red;
	right->material.Color = green;
	bottom->material.Color = white;
	back->material.Color = white;
	top->material.Color = white;

	//top->material.Specularity = 1.0f;

	Plane* light = new Plane(vec3(0.4f, 0.999f, 0.6f), vec3(0.6f, 0.999f, 0.6f), vec3(0.4f, 0.999f, 0.4f));
	light->material.isEmissive = true;

	Sphere* metal = new Sphere(vec3(0.3, 0.15f, 0.7), 0.15f);
	metal->material.Specularity = 1.0f;

	Sphere* glass2 = new Sphere(vec3(0.725, 0.15f, 0.3f), 0.15f);
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

		if (record.Primitive->material.isDielectric)
		{
			float reflectance = Fresnel(ray.Direction, record.Normal, 1.5f);
			float transmittance = 1.0f - reflectance;

			outputColor += reflectance * IndirectIllumination(record, ray, 1);
			outputColor += transmittance * RefractionIllumination(record, ray, 1);
		}
		else
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

	float decrease = 1.0f;

	if (shadowRecord.t < r)
	{
		if (!shadowRecord.Primitive->material.isDielectric)
		{
			// Direct light is blocked by another surface
			return vec3(0.0f);
		}
		else
		{
			decrease = 0.65f;
		}
	}

	float diff = max(Dot(record.Normal, lightDir), 0.0);
	vec3 lightC = lightColor * min((lightIntensity / r2), 1.0f);

	return record.Primitive->material.Color * lightC * diff * decrease;
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

		if(reflectRecord.Primitive->material.isDielectric)
		{
			float reflectance = Fresnel(reflectedRay.Direction, reflectRecord.Normal, 1.5f);
			float transmittance = 1.0f - reflectance;

			illumination += reflectance * IndirectIllumination(reflectRecord, reflectedRay, currentRayDepth);
			illumination += transmittance * RefractionIllumination(reflectRecord, reflectedRay, currentRayDepth);
		}
		else
		{
			float diffuse = 1.0f - reflectRecord.Primitive->material.Specularity;
			float specular = reflectRecord.Primitive->material.Specularity;

			if(diffuse > 0.0f)
			{
				illumination += diffuse * materialColor * DirectIllumination(reflectRecord);
			}

			if(specular > 0.0f)
			{
				illumination += specular * materialColor * IndirectIllumination(reflectRecord, reflectedRay, currentRayDepth);
			}
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
	vec3 a = vec3(0.8f, 0.261f, 0.6f);
	vec3 b = vec3(0.475f, 0.91f, 1.0f);

	float t = max(ray.Direction.y, 0.0);

	return (1.0f - t) * a + b * t;
}

vec3 RayTracer::RefractionIllumination(const HitRecord& record, const Ray& ray, int rayDepth)
{
	if (rayDepth >= maxRayDepth)
	{
		return vec3(0.0f);
	}

	int currentRayDepth = rayDepth + 1;
	vec3 materialColor = record.Primitive->material.Color;

	vec3 rf = Refract(ray.Direction, record.Normal, 1.5f);
	Ray refractRay = Ray(record.HitPoint + rf * EPSILON, rf);

	HitRecord refractRecord;
	refractRecord.t = maxT;
	refractRecord.InsideMedium = record.InsideMedium;

	IntersectScene(refractRay, refractRecord);

	if (refractRecord.t < maxT)
	{
		vec3 illumination(0.0f);

		if (refractRecord.Primitive->material.isDielectric)
		{
			float reflectance = Fresnel(refractRay.Direction, refractRecord.Normal, 1.5f);
			float transmittance = 1.0f - reflectance;

			illumination += reflectance * IndirectIllumination(refractRecord, refractRay, currentRayDepth);
			illumination += transmittance * RefractionIllumination(refractRecord, refractRay, currentRayDepth);
		}
		else
		{
			float diffuse = 1.0f - refractRecord.Primitive->material.Specularity;
			float specular = refractRecord.Primitive->material.Specularity;

			if (diffuse > 0.0f)
			{
				illumination += diffuse * materialColor * DirectIllumination(refractRecord);
			}

			if (specular > 0.0f)
			{
				illumination += specular * materialColor * IndirectIllumination(refractRecord, refractRay, currentRayDepth);
			}
		}

		// for now we don't step further, do in a minute
		return illumination;
	}

	return materialColor * GetSkyColor(refractRay);
}
