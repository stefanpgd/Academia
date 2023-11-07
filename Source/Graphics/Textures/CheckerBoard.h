#pragma once
#include "Graphics/Texture.h"

class CheckerBoard : public Texture
{
public:
	CheckerBoard(vec3 colorA, vec3 colorB, float scale);

	vec3 Sample(const HitRecord& record) override;

private:
	vec3 colorA;
	vec3 colorB;
	float scale;
};