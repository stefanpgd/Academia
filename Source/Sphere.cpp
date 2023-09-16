#include "Sphere.h"
#include <cmath>

Sphere::Sphere(vec3 position, float radius) : Radius(radius), Radius2(radius * radius) 
{
	Position = position;
	material.Color = vec3(1.0f);
}

Sphere::Sphere(vec3 position, float radius, vec3 color) : Radius(radius), Radius2(radius * radius)
{
	Position = position;
	material.Color = color;
}

void Sphere::Intersect(const Ray& ray, HitRecord& record)
{
	float projection = Dot(Position - ray.Origin, ray.Direction); // projection value
	vec3 closestPoint = ray.At(projection); // closest point to sphere center along ray dir
	float dist = (Position - closestPoint).Magnitude();

	// Outside sphere, so return as miss.
	if (dist > Radius)
	{
		record.t = -1.0f;
		return;
	}

	// the intersection point, sphere center and closest point form a triangle.
	// we already know the opposite (dist) and the hypotenuse, which is always sphere radius.
	// so a2 + b2 = c2, which also means c2 - b2 = a2;
	float insideLength = sqrtf(Radius2 - dist * dist);

	record.t = projection - insideLength;
	record.HitPoint = ray.At(record.t);
	record.Normal = Normalize(record.HitPoint - Position);
	record.Primitive = this;
}