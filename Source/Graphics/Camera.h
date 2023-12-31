#pragma once
#include "Math/MathCommon.h"

class Camera
{
public:
	Camera(unsigned int screenWidth, unsigned int screenHeight);
	Camera(vec3 position, unsigned int screenWidth, unsigned int screenHeight);

	bool Update(float deltaTime);
	void SetupVirtualPlane(unsigned int screenWidth, unsigned int screenHeight);

	Ray GetRay(int pixelX, int pixelY);

public:
	// Orientation //
	vec3 Position = vec3(0.0f, 0.4f, 0.0f);
	vec3 ViewDirection;

	float Speed = 0.25f;
	float BoostMultiplier = 2.5f; // Holding Shift
	float SlowMultiplier = 0.15f; // Holding Ctrl

private:
	// Screen virtual plane //
	vec3 screenCenter;
	vec3 screenP0, screenP1, screenP2;
	vec3 screenU, screenV;
	unsigned int screenWidth, screenHeight;
	float pixelSizeX, pixelSizeY;
};