#include "App.h"
#include "Utilities/Utilities.h"
#include "Math/MathCommon.h"

#include <GLFW/glfw3.h>
#include <cassert>
#include <vector>

#include "Sphere.h"

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

	std::vector<Primitive*> primitives;
	primitives.push_back(new Sphere(vec3(1.5f, 0.0f, 4.0f), 0.5f));
	primitives.push_back(new Sphere(vec3(4.5f, 0.0f, 6.0f), 0.15f));

	const float maxDepth = 10000.0f;

	while (runApp)
	{
		frameCount++;

		glClear(GL_COLOR_BUFFER_BIT);

		for (int x = 0; x < screenWidth; x++)
		{
			for (int y = 0; y < screenHeight; y++)
			{
				// determine where on the virtual screen we need to be //
				float xScale = x / float(screenWidth);
				float yScale = y / float(screenHeight);

				// Using our screen vectors, determine our ray direction //
				vec3 screenPoint = screenP0;
				screenPoint += uDir * xScale;
				screenPoint += vDir * yScale;

				vec3 rayDir = screenPoint - camera;
				rayDir.Normalize();

				Ray ray = Ray(camera, rayDir);

				screenBuffer[x + y * screenWidth] = 0x00;

				float closestT = maxDepth;
				int primI = -1;

				for (Primitive* prim : primitives)
				{
					float t = prim->Intersect(ray);

					if (t > 0.0f && t < closestT)
					{
						closestT = t;
					}
				}

				if (closestT != maxDepth)
				{
					screenBuffer[x + y * screenWidth] = 0xff;
				}
			}
		}

		// draw buffer into screen buffer
		glDrawPixels(screenWidth, screenHeight, GL_RGBA, GL_UNSIGNED_BYTE, screenBuffer);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}