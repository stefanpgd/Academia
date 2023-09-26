#include "App.h"
#include "Utilities/Utilities.h"

#include <cassert>
#include <vector>
#include <chrono>

#include "Input.h"
#include "Editor.h"

#include "Graphics/RayTracer.h"
#include "Utilities/Timer.h"

void GLFWErrorCallback(int, const char* err_str)
{
	LOG(Log::MessageType::Error, err_str);
}

App::App()
{
	bufferSize = screenWidth * screenHeight;
	screenBuffer = new unsigned int[bufferSize];
	colorBuffer = new vec3[bufferSize];
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

	LOG("Succesfully created a window.");
	glfwMakeContextCurrent(window);

	// Start initializing custom systems  //
	Input::Initialize(window);
	Editor::Initialize(window);

	rayTracer = new RayTracer(screenWidth, screenHeight);

	LOG("'Academia' has succesfully initialized!");
}

App::~App()
{
	// Destroy tracer enc. enc.
	glfwDestroyWindow(window);
}

void App::Run()
{
	static float deltaTime = 0.0f;
	static std::chrono::high_resolution_clock clock;
	static auto t0 = std::chrono::time_point_cast<std::chrono::milliseconds>((clock.now())).time_since_epoch();

	while (runApp)
	{
		auto t1 = std::chrono::time_point_cast<std::chrono::milliseconds>((clock.now())).time_since_epoch();
		deltaTime = (t1 - t0).count() * .001;
		t0 = t1;

		Start();
		Update(deltaTime);
		Render();

		glfwPollEvents();
		if(glfwWindowShouldClose(window))
		{
			runApp = false;
		}

		frameCount++;
	}
}

void App::Start()
{
	glClear(GL_COLOR_BUFFER_BIT);

	Editor::Start();
}

void App::Update(float deltaTime)
{
	bool sceneUpdated = false;

	sceneUpdated = rayTracer->Update(deltaTime);

	Editor::Update();

	if(sceneUpdated)
	{
		ClearScreenbuffers();
	}

	// Process input
	// Process editor & Scene stuff...
	// Update components like camera

	// Potentially reset buffers like colorBuffer incase scene got updated
}

void App::Render()
{
	for(int x = 0; x < screenWidth; x++)
	{
		for(int y = 0; y < screenHeight; y++)
		{
			int i = x + y * screenWidth;
			colorBuffer[i] += rayTracer->Trace(x, y, maxRayDepth);

			vec3 output = colorBuffer[i];
			output = output * (1.0f / (float)frameCount);

			screenBuffer[i] = AlbedoToRGB(output.x, output.y, output.z);
		}
	}

	// Copy screen-buffer data over to the Window's buffer.
	glDrawPixels(screenWidth, screenHeight, GL_RGBA, GL_UNSIGNED_BYTE, screenBuffer);

	Editor::Render();
	glfwSwapBuffers(window);
}

void App::ClearScreenbuffers()
{
	frameCount = 1;
	ClearBuffer(screenBuffer, 0x00, bufferSize);
	memset(colorBuffer, 0.0f, sizeof(vec3) * bufferSize);
}