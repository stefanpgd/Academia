#pragma once

class Ray;

class Primitive
{
public:
	virtual float Intersect(const Ray& ray) = 0;
};