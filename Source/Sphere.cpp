#include "Sphere.h"
#include <cmath>

Sphere::Sphere(vec3 position, float radius) : Position(position), Radius(radius), Radius2(radius * radius) {}

float Sphere::Intersect(const Ray& ray)
{
	float projection = Dot(Position - ray.Origin, ray.Direction); // projection value
	vec3 closestPoint = ray.At(projection); // closest point to sphere center along ray dir
	float dist = (Position - closestPoint).Magnitude();

	// Outside sphere, so return as miss.
	if (dist > Radius)
	{
		return -1.0f;
	}

	// the intersection point, sphere center and closest point form a triangle.
	// we already know the opposite (dist) and the hypotenuse, which is always sphere radius.
	// so a2 + b2 = c2, which also means c2 - b2 = a2;
	float insideLength = sqrtf(Radius2 - dist * dist);
	
	//vec3 t1 = ray.At(projection - insideLength);
	return insideLength;
}