#include "App.h"
#include "Utilities/Utilities.h"

#include <cassert>
#include <vector>
#include <chrono>

#include "Input.h"
#include "Editor.h"
#include "SceneManager.h"
#include "Graphics/RayTracer.h"
#include "Utilities/Timer.h"

void GLFWErrorCallback(int, const char* err_str)
{
	LOG(Log::MessageType::Error, err_str);
}

void GLFWWindowResize(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
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
	glfwSetWindowSizeCallback(window, GLFWWindowResize);

	// Start initializing custom systems  //
	Input::Initialize(window);

	sceneManager = new SceneManager();
	editor = new Editor(window);
	editor->SetActiveScene(sceneManager->GetActiveScene());

	rayTracer = new RayTracer(screenWidth, screenHeight, sceneManager->GetActiveScene());

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

	editor->Start();

	int width, height;
	glfwGetWindowSize(window, &width, &height);

	if(width != screenWidth || height != screenHeight)
	{
		ResizeBuffers(width, height);
	}
}

void App::Update(float deltaTime)
{
	bool cameraUpdated = false;
	bool sceneUpdated = false;

	cameraUpdated = rayTracer->Update(deltaTime);
	sceneUpdated = editor->Update(deltaTime);

	if(sceneUpdated || cameraUpdated)
	{
		ClearScreenbuffers();
	}

	ImGui::Begin("Post Process");
	ImGui::DragFloat("Exposure", &exposure, 0.02f, 0.0f, 5.0f);
	ImGui::End();

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
			colorBuffer[i] += rayTracer->Trace(x, y);

			vec3 output = colorBuffer[i];
			output = output * (1.0f / (float)frameCount);

			vec3 temp = output * exposure;

			const float a = 2.51f;
			const float b = 0.03f;
			const float c = 2.43f;
			const float d = 0.59f;
			const float e = 0.14f;
			vec3 a1 = (temp * (a * temp + b));
			vec3 b1 = (temp * (c * temp + d) + e);
			temp.x = a1.x / b1.x;
			temp.y = a1.y / b1.y;
			temp.z = a1.z / b1.z;
			temp.x = Clamp(temp.x, 0.0f, 1.0f);
			temp.y = Clamp(temp.y, 0.0f, 1.0f);
			temp.z = Clamp(temp.z, 0.0f, 1.0f);
			output = temp;

			screenBuffer[i] = AlbedoToRGB(output.x, output.y, output.z);
		}
	}

	// Copy screen-buffer data over to the Window's buffer.
	glDrawPixels(screenWidth, screenHeight, GL_RGBA, GL_UNSIGNED_BYTE, screenBuffer);

	editor->Render();

	glfwSwapBuffers(window);
}

void App::ResizeBuffers(int width, int height)
{
	screenWidth = width;
	screenHeight = height;

	bufferSize = screenWidth * screenHeight;
	delete screenBuffer;
	delete colorBuffer;

	screenBuffer = new unsigned int[bufferSize];
	colorBuffer = new vec3[bufferSize];
	ClearScreenbuffers();

	// Update rendering side //
	rayTracer->Resize(width, height);
}

void App::ClearScreenbuffers()
{
	frameCount = 1;
	ClearBuffer(screenBuffer, 0x00, bufferSize);
	memset(colorBuffer, 0.0f, sizeof(vec3) * bufferSize);
}