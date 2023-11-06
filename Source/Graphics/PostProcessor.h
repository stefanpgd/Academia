#pragma once
#include "Math/Vec3.h"

class PostProcessor
{
public:
	PostProcessor();

	unsigned int PostProcess(vec3& color);

private:
	float gammaInverse = 1.0f;
	float gamma = 2.4f;
};