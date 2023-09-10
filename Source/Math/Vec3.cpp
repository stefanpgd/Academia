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

vec3 Reflect(const vec3& in, const vec3& normal)
{
	vec3 v = normal * Dot(in, normal);
	vec3 u = in - v;
	return u - v;
}

vec3 Normalize(const vec3& n)
{
	float InvMag = 1.0 / n.Magnitude();
	return vec3(n.x * InvMag, n.y * InvMag, n.z * InvMag);
}
