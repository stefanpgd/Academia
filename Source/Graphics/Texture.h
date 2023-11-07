#pragma once
#include "Graphics/Primitive.h"

/// <summary>
/// Textures give the opportunity to replace the simple Albedo
/// with a custom color based on the values of the passed along hit record.
/// </summary>
class Texture
{
public:
	virtual vec3 Sample(const HitRecord& record) = 0;
};