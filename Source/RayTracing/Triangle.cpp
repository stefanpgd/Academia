#include "Triangle.h"
#include <cmath>

Triangle::Triangle(vec3 v0, vec3 v1, vec3 v2) : v0(v0), v1(v1), v2(v2)
{
	e1 = v1 - v0;
	e2 = v2 - v0;
	Normal = Normalize(Cross(e1, e2)) * -1.0f;
}

void Triangle::Intersect(const Ray& ray, HitRecord& record)
{
	vec3 h = Cross(ray.Direction, e2);
	float area = Dot(e1, h);

	if (area > -EPSILON && area < EPSILON)
	{
		record.t = -1.0f;
		return;
	}

	float f = 1.0f / area;
	vec3 s = ray.Origin - v0;
	float weightU = f * Dot(s, h);

	if (weightU < 0.0f || weightU > 1.0)
	{
		// Weights don't add up to 1, thus the point is outside te triangle.
		record.t = -1.0f;
		return;
	}

	vec3 q = Cross(s, e1);
	float weightV = f * Dot(ray.Direction, q);

	if (weightV < 0.0f || weightU + weightV > 1.0)
	{
		// Weights don't add up to 1, thus the point is outside te triangle.
		record.t = -1.0f;
		return;
	}

	float t = f * Dot(e2, q);

	if (t > EPSILON)
	{
		record.t = t;
		record.HitPoint = ray.At(t);
		record.Normal = Normal;
		record.Primitive = this;
		return;
	}

	record.t = -1.0f;
	return;
}