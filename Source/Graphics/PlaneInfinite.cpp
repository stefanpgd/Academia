#include "PlaneInfinite.h"
#include <cmath>

PlaneInfinite::PlaneInfinite(vec3 position, vec3 normal)
{
	Position = position;
	Normal = normal;
}

void PlaneInfinite::Intersect(const Ray& ray, HitRecord& record)
{
	float denom = Dot(Normal, ray.Direction);

	if (abs(denom) > 1e-6)
	{
		vec3 pToIntersect = Position - ray.Origin;
		float d = Dot(pToIntersect, Normal);
		float t = d / denom;

		if (t >= 0.0f)
		{
			record.t = t;
			record.HitPoint = ray.At(t);
			record.Normal = Normal;
			record.Primitive = this;
			return;
		}
	}

	record.t = -1.0f;
	return;
}