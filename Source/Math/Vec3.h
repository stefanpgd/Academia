#pragma once

struct Vec3
{
public:
	Vec3();
	Vec3(float v);
	Vec3(float x, float y, float z);

	Vec3(const Vec3& rh);
	Vec3& operator=(const Vec3& rh);
	void operator+=(const Vec3& rh);

	float Magnitude() const;
	void Normalize();

	float x, y, z;
};

typedef Vec3 vec3;

inline vec3 operator+(const vec3& lh, const vec3& rh) { return vec3(lh.x + rh.x, lh.y + rh.y, lh.z + rh.z); }
inline vec3 operator-(const vec3& lh, const vec3& rh) { return vec3(lh.x - rh.x, lh.y - rh.y, lh.z - rh.z); }
inline vec3 operator*(const vec3& lh, float rh) { return vec3(lh.x * rh, lh.y * rh, lh.z * rh); }

float Dot(const vec3& lh, const vec3& rh);
vec3 Cross(const vec3& lh, const vec3& rh);
vec3 Reflect(const vec3& in, const vec3& normal);
vec3 Normalize(const vec3& n);

// To do:
// Operator overloading
// union
// proper copy & assignment constructor