#include "Vec3.h"
#include <iostream>
#include "../Utilities/Utilities.h"

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
	float cosI = Dot(in, normal);
	float n1 = 1.0f;
	float n2 = IoR;
	vec3 norm = normal;

	if (cosI < 0.0f)
	{
		// Going from air into medium
		cosI = -cosI;
	}
	else
	{
		// Going from medium back into air
		float t = n1;
		n1 = n2;
		n2 = t;
		norm = norm * -1.0f;
	}

	float eta = n1 / n2;
	float k = 1.0f - eta * eta * (1.0f - cosI * cosI);

	if (k < 0.0f)
	{
		return Reflect(in, normal);
	}

	vec3 a = eta * (in + (cosI * norm));
	vec3 b = (norm * -1.0f) * sqrtf(k);

	return a + b;
}

vec3 Normalize(const vec3& n)
{
	float InvMag = 1.0 / n.Magnitude();
	return vec3(n.x * InvMag, n.y * InvMag, n.z * InvMag);
}

// Returns the amount of Reflectance from fresnel
// using this value, compute the transmitance by doing: '1.0 - Reflectance'
float Fresnel(const vec3& in, const vec3& normal, float IoR)
{
	float cosI = Dot(in, normal);
	float n1 = 1.0f;
	float n2 = IoR;

	if (cosI > 0.0f)
	{
		float t = n1;
		n1 = n2;
		n2 = t;
	}

	float sinR = n1 / n2 * sqrtf(fmaxf(1.0f - cosI * cosI, 0.0f));
	if (sinR >= 1.0f)
	{
		// TIR, aka perfect reflectance, which happens at the exact edges of a surface.
		return 1.0f;
	}

	float cosR = sqrtf(fmaxf(1.0f - sinR * sinR, 0.0f));
	cosI = fabsf(cosI);

	float Fp = (n2 * cosI - n1 * cosR) / (n2 * cosI + n1 * cosR);
	float Fr = (n1 * cosI - n2 * cosR) / (n1 * cosI + n2 * cosR);

	return (Fp * Fp + Fr * Fr) * 0.5f;
}

vec3 RandomUnitVector()
{
	while(true)
	{
		vec3 v = vec3(RandomInRange(-1.0f, 1.0f), RandomInRange(-1.0f, 1.0f), RandomInRange(-1.0f, 1.0f));
		
		// ensures there is no bias for the corners
		if(v.MagnitudeSquared() < 1)
		{
			return Normalize(v);
		}
	}
}