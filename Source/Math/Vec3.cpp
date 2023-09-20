#include "Vec3.h"
#include <iostream>

Vec3::Vec3() : x(0.0f), y(0.0f), z(0.0f) {}
Vec3::Vec3(float v) : x(v), y(v), z(v) {}
Vec3::Vec3(float x, float y, float z) : x(x), y(y), z(z) {}
Vec3::Vec3(const Vec3& rh) : x(rh.x), y(rh.y), z(rh.z) {}

Vec3& Vec3::operator=(const Vec3& rh)
{
	x = rh.x;
	y = rh.y;
	z = rh.z;

	return *this;
}

void Vec3::operator+=(const Vec3& rh)
{
	x += rh.x;
	y += rh.y;
	z += rh.z;
}

float Vec3::Magnitude() const
{
	return sqrtf(x * x + y * y + z * z);
}

float Vec3::MagnitudeSquared() const
{
	return x * x + y * y + z * z;
}

void Vec3::Normalize()
{
	float InvMag = 1.0 / Magnitude();

	x *= InvMag;
	y *= InvMag;
	z *= InvMag;
}

float Dot(const vec3& lh, const vec3& rh)
{
	return lh.x * rh.x + lh.y * rh.y + lh.z * rh.z;
}

vec3 Cross(const vec3& lh, const vec3& rh)
{
	float x = lh.y * rh.z - lh.z * rh.y;
	float y = lh.z * rh.x - lh.x * rh.z;
	float z = lh.x * rh.y - lh.y * rh.x;

	return vec3(x, y, z);
}

vec3 Reflect(const vec3& in, const vec3& normal)
{
	vec3 v = normal * Dot(in, normal);
	vec3 u = in - v;
	return u - v;
}

vec3 Refract(const vec3& in, const vec3& normal, float IoR)
{
	float eta1 = 1.0f; // IoR of Air
	float eta2 = IoR;
	float theta = Dot(in, normal);
	vec3 norm = normal;

	if (theta > 0.0f)
	{
		norm = normal * -1.0f;
		float temp = eta1;
		eta1 = eta2;
		eta2 = temp;
	}
	else
	{
		theta = -theta;
	}


	float eta = eta1 / eta2;
	float k = 1 - eta * eta * (1 - theta * theta);

	if (k < 0.0f)
	{
		return in;
	}

	vec3 a = eta * (in + (norm * theta));
	vec3 b = sqrtf(1.0f - (eta * eta) * (1.0f - theta * theta)) * (norm * -1.0f);
	return a + b;
}

vec3 Normalize(const vec3& n)
{
	float InvMag = 1.0 / n.Magnitude();
	return vec3(n.x * InvMag, n.y * InvMag, n.z * InvMag);
}
