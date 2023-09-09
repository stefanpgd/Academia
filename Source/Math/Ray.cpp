#include "Ray.h"

Ray::Ray() : Origin(vec3(0.0)), Direction(vec3(0.0)) {}
Ray::Ray(const vec3& origin, const vec3& direction) : Origin(origin), Direction(direction) {}

Vec3 Ray::At(float t)
{
	return Origin + Direction * t;
}