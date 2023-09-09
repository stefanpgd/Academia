#include "App.h"
#include "Utilities/Utilities.h"
#include "Math/MathCommon.h"

#include <GLFW/glfw3.h>
#include <cassert>

void GLFWErrorCallback(int, const char* err_str)
{
	LOG(Log::MessageType::Error, err_str);
}

App::App()
{
	bufferSize = screenWidth * screenHeight;
	screenBuffer = new unsigned int[bufferSize];
	ClearBuffer(screenBuffer, 0x00, bufferSize);

	glfwSetErrorCallback(GLFWErrorCallback);

	if (!glfwInit())
	{
		// Check out again how to retrieve errors from GLFW
		LOG(Log::MessageType::Error, "GLFW didn't intialize properly");
		assert(false);
	}
	
	LOG("Succesfully initialized GLFW.");
	window = glfwCreateWindow(screenWidth, screenHeight, appName.c_str(), NULL, NULL);

	if (!window)
	{
		LOG(Log::MessageType::Error, "Failed to create a GLFW window");
		assert(false);
	}

	LOG("Succesfully created a window");
	glfwMakeContextCurrent(window);
}

void App::Run()
{
	// LH system
	vec3 camera = Vec3(0.0f);
	vec3 viewDir = Vec3(0.0f, 0.0f, 1.0f);

	vec3 screenCenter = camera + viewDir;
	
	vec3 screenP0 = screenCenter + vec3(-0.5f, -0.5f, 0.0f);	// Bottom Left
	vec3 screenP1 = screenCenter + vec3(0.5, -0.5, 0.0f);		// Bottom Right
	vec3 screenP2 = screenCenter + vec3(-0.5f, 0.5f, 0.0f);		// Top left

	float aspect = screenWidth / float(screenHeight);
	vec3 uDir = (screenP1 - screenP0) * aspect;
	vec3 vDir = screenP2 - screenP0;

	int frameCount = 0;

	while (runApp)
	{
		frameCount++;

		glClear(GL_COLOR_BUFFER_BIT);

		for (int x = 0; x < screenWidth; x++)
		{
			for (int y = 0; y < screenHeight; y++)
			{
				float xScale = x / float(screenWidth);
				float yScale = y / float(screenHeight);

				vec3 screenPoint = screenP0;
				screenPoint += uDir * xScale;
				screenPoint += vDir * yScale;

				vec3 rayDir = screenPoint - camera;
				rayDir.Normalize();

				Ray ray = Ray(camera, rayDir);

				float sphereRad = 0.5;
				vec3 spherePos = vec3(1.5f, 0.0f, 4.0f);

				float t = Dot(spherePos - ray.Origin, ray.Direction);
				vec3 p = ray.At(t);
				float d = (spherePos - p).Magnitude();

				screenBuffer[x + y * screenWidth] = 0x00;

				if (d < sphereRad)
				{
					float insideLength = sqrtf(sphereRad * sphereRad - d * d);
					vec3 t1 = ray.At(t - insideLength);
					vec3 normal = (t1 - spherePos);
					normal.Normalize();

					vec3 lightP = spherePos + vec3(2.0f, 2.0f, -3.0f);
					vec3 lightD = lightP - t1;
					lightD.Normalize();

					float c = max(Dot(lightD, normal), 0.0);

					vec3 hitToEye = camera - t1;
					hitToEye.Normalize();

					vec3 LightDIn = t1 - lightP;
					LightDIn.Normalize();
					float spec = pow(Dot(hitToEye, Reflect(LightDIn, normal)), 512.0);

					if (c > 0.0)
					{
						c += spec;
					}
					c += 0.075; // ambient
					c = min(c, 1.0);

					int lightS = 255.0f * max(c, 0.0);
					unsigned int outputC = (lightS << 16) + (int(float(lightS)) << 8) + lightS;

					screenBuffer[x + y * screenWidth] = outputC;
				}
			}
		}

		// draw buffer into screen buffer
		glDrawPixels(screenWidth, screenHeight, GL_RGBA, GL_UNSIGNED_BYTE, screenBuffer);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}