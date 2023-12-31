#include "Camera.h"
#include "Utilities/Utilities.h"
#include "Framework/Input.h"

Camera::Camera(unsigned int screenWidth, unsigned int screenHeight)
{
	// LH system
	Position = Vec3(0.5, 0.5f, -1.0f);
	ViewDirection = Vec3(0.0f, 0.0f, 1.0f);

	SetupVirtualPlane(screenWidth, screenHeight);
}

Camera::Camera(vec3 position, unsigned int screenWidth, unsigned int screenHeight) : Position(position)
{
	ViewDirection = Vec3(0.0f, -0.15f, 2.0f);

	SetupVirtualPlane(screenWidth, screenHeight);
}

bool Camera::Update(float deltaTime)
{
	bool cameraUpdated = false;
	float speed = Speed * deltaTime;

	if(Input::GetKey(KeyCode::LeftShift))
	{
		speed *= BoostMultiplier;
	}

	if(Input::GetKey(KeyCode::LeftCtrl))
	{
		speed *= SlowMultiplier;
	}

	int forward = Input::GetKey(KeyCode::W) - Input::GetKey(KeyCode::S);
	if(forward != 0.0)
	{
		// in the need of delta time...
		Position.z += speed * forward;
		cameraUpdated = true;
	}

	int sideward = Input::GetKey(KeyCode::D) - Input::GetKey(KeyCode::A);
	if(sideward != 0.0)
	{
		// in the need of delta time...
		Position.x += speed * sideward;
		cameraUpdated = true;
	}

	int upwards = Input::GetKey(KeyCode::E) - Input::GetKey(KeyCode::Q);
	if(upwards != 0.0)
	{
		Position.y += speed * upwards;
		cameraUpdated = true;
	}

	if(cameraUpdated)
	{
		SetupVirtualPlane(screenWidth, screenHeight);
	}

	return cameraUpdated;
}

void Camera::SetupVirtualPlane(unsigned int screenWidth, unsigned int screenHeight)
{
	this->screenWidth = screenWidth;
	this->screenHeight = screenHeight;

	float aspectRatio = float(screenWidth) / float(screenHeight);
	float xOffset = (aspectRatio - 1.0f) * 0.5f;

	screenCenter = Position + ViewDirection;

	screenP0 = screenCenter + vec3(-0.5f - xOffset, -0.5f, 0.0f);		// Bottom Left
	screenP1 = screenCenter + vec3(0.5f + xOffset, -0.5, 0.0f);		// Bottom Right
	screenP2 = screenCenter + vec3(-0.5f - xOffset, 0.5f, 0.0f);		// Top left

	screenU = screenP1 - screenP0;
	screenV = screenP2 - screenP0;

	pixelSizeX = (1.0f / float(screenWidth)) * 0.5f;
	pixelSizeY = (1.0f / float(screenHeight)) * 0.5f;
}

Ray Camera::GetRay(int pixelX, int pixelY)
{
	// determine where on the virtual screen we need to be //
	// Note: position is treated as the top-left corner of a pixel 
	float posX = pixelX / float(screenWidth);
	float posY = pixelY / float(screenHeight);

	// Anti-Aliasing (Monte-Carlo)
	posX += RandomInRange(-pixelSizeX, pixelSizeX);
	posY += RandomInRange(-pixelSizeY, pixelSizeY);

	vec3 screenPoint = screenP0 + (screenU * posX) + (screenV * posY);
	vec3 rayDirection = Normalize(screenPoint - Position);

	return Ray(Position, rayDirection);
}