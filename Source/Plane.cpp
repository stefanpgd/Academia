#include "Plane.h"
#include <cmath>

Plane::Plane(vec3 v0, vec3 v1, vec3 v2) : v0(v0)
{
	u = v1 - v0;
	v = v2 - v0;

	Normal = Normalize(Cross(u, v)) * -1.0f;
	Position = v0 + u * 0.5f + v * 0.5f;

	w = u.Magnitude();
	h = v.Magnitude();
	u = Normalize(u);
	v = Normalize(v);
}

void Plane::Intersect(const Ray& ray, HitRecord& record)
{
	float denom = Dot(Normal, ray.Direction);

	if (abs(denom) > 1e-6)
	{
		vec3 pToIntersect = Position - ray.Origin;
		float d = Dot(pToIntersect, Normal);
		float t = d / denom;

		if (t >= 0.0f)
		{
			vec3 i = ray.At(t);
			vec3 v0ToI = i - v0;

			float dU = Dot(v0ToI, u);
			if (dU < 0.0f || dU > 1.0f)
			{
				record.t = -1.0f;
				return;
			}

			float dV = Dot(v0ToI, v);
			if (dV < 0.0f || dV > 1.0f)
			{
				record.t = -1.0f;
				return;
			}

			if (abs(dU) < w && abs(dV) < h)
			{
				// We are inside of the boundaries of the plane
				record.t = t;
				record.HitPoint = i;
				record.Normal = Normal;
				record.Primitive = this;
				return;
			}
		}
	}

	record.t = -1.0f;
	return;
}