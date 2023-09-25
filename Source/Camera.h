#pragma once
#include "Math/MathCommon.h"

class Camera
{
public:
	Camera(unsigned int screenWidth, unsigned int screenHeight);

	bool Update();
	void SetupVirtualPlane(unsigned int screenWidth, unsigned int screenHeight);

	Ray GetRay(int pixelX, int pixelY);

public:
	// Orientation //
	vec3 Position;
	vec3 ViewDirection;

	float Speed = 0.025f;

private:
	// Screen virtual plane //
	vec3 screenCenter;
	vec3 screenP0, screenP1, screenP2;
	vec3 screenU, screenV;
	unsigned int screenWidth, screenHeight;
	float pixelSizeX, pixelSizeY;
};