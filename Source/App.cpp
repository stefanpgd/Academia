#include "App.h"
#include "Utilities.h"

#include <GLFW/glfw3.h>
#include <cassert>

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
	while (runApp)
	{
		glClear(GL_COLOR_BUFFER_BIT);

		// DRAW UV SPACE
		for (int y = 0; y < screenHeight; y++)
		{
			for (int x = 0; x < screenWidth; x++)
			{
				int x_t = ((float)x / (float)screenWidth) * 255.0f;
				int y_t = ((float)y / (float)screenHeight) * 255.0f;

				screenBuffer[x + y * screenWidth] = (uint32_t)((0x00000100 * x_t) | (0x00000001 * y_t));
			}
		}

		// draw buffer into screen buffer
		glDrawPixels(screenWidth, screenHeight, GL_RGBA, GL_UNSIGNED_BYTE, screenBuffer);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}