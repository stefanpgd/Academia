#include "Camera.h"
#include "Utilities/Utilities.h"
#include "Input.h"

Camera::Camera(unsigned int screenWidth, unsigned int screenHeight) : screenWidth(screenWidth), screenHeight(screenHeight)
{
	// LH system
	Position = Vec3(0.5, 0.5f, -1.0f);
	ViewDirection = Vec3(0.0f, 0.0f, 1.f);

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
		speed *= SlowMultilplier;
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

	if(cameraUpdated)
	{
		SetupVirtualPlane(screenWidth, screenHeight);
	}

	return cameraUpdated;
}

void Camera::SetupVirtualPlane(unsigned int screenWidth, unsigned int screenHeight)
{
	screenCenter = Position + ViewDirection;

	screenP0 = screenCenter + vec3(-0.5f, -0.5f, 0.0f);		// Bottom Left
	screenP1 = screenCenter + vec3(0.5, -0.5, 0.0f);		// Bottom Right
	screenP2 = screenCenter + vec3(-0.5f, 0.5f, 0.0f);		// Top left

	screenU = screenP1 - screenP0;
	screenV = screenP2 - screenP0;

	pixelSizeX = (1.0f / float(screenWidth)) * 0.5f;
	pixelSizeY = (1.0f / float(screenHeight)) * 0.5f;
}

Ray Camera::GetRay(int pixelX, int pixelY)
{
	// determine where on the virtual screen we need to be //
	float xScale = pixelX / float(screenWidth);
	float yScale = pixelY / float(screenHeight);

	// Anti-Aliasing (Monte-Carlo? - Double check the lecture)
	xScale += RandomInRange(-pixelSizeX, pixelSizeX);
	yScale += RandomInRange(-pixelSizeY, pixelSizeY);

	vec3 screenPoint = screenP0 + (screenU * xScale) + (screenV * yScale);
	vec3 rayDirection = Normalize(screenPoint - Position);

	return Ray(Position, rayDirection);
}