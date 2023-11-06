#include "Renderer.h"

#include <GLFW/glfw3.h>
#include <cassert>
#include <imgui.h>
#include <stb_image_write.h>

#include "Input.h"
#include "SceneManager.h"
#include "WorkerSystem.h"
#include "Graphics/RayTracer.h"
#include "Utilities/Utilities.h"

Renderer::Renderer(const std::string& windowName, unsigned int screenWidth, unsigned int screenHeight) :
	screenWidth(screenWidth), screenHeight(screenHeight)
{
	// Create Back Buffers // 
	bufferSize = screenWidth * screenHeight;
	screenBuffer = new unsigned int[bufferSize];
	sampleBuffer = new vec3[bufferSize];
	ClearBuffer(screenBuffer, 0x00, bufferSize);

	// Initialize GLFW & Window // 
	if(!glfwInit())
	{
		// Check out again how to retrieve errors from GLFW
		LOG(Log::MessageType::Error, "GLFW didn't intialize properly");
		assert(false);
	}

	LOG("Succesfully initialized GLFW.");
	window = glfwCreateWindow(screenWidth, screenHeight, windowName.c_str(), NULL, NULL);

	if(!window)
	{
		LOG(Log::MessageType::Error, "Failed to create a GLFW window!");
		assert(false);
	}

	LOG("Succesfully created a window.");
	glfwMakeContextCurrent(window);

	// Intialize Scene & Ray Tracer //
	sceneManager = new SceneManager(screenWidth, screenHeight);
	rayTracer = new RayTracer(screenWidth, screenHeight, sceneManager->GetActiveScene());
	workerSystem = new WorkerSystem(this, screenWidth, screenHeight);
}

Renderer::~Renderer()
{
	delete sceneManager;

	glfwDestroyWindow(window);
}

void Renderer::Start()
{
	glClear(GL_COLOR_BUFFER_BIT);

	int width, height;
	glfwGetWindowSize(window, &width, &height);

	if(width != screenWidth || height != screenHeight)
	{
		resizeScreenBuffers = true;
	}
}

void Renderer::Update()
{
	workerSystem->Update();

	// In case we reached our target frame count
	// we want to check if either something got updated or resized
	// If so, we force the screen to update again, and the path tracer restarts.
	if(sampleCount >= targetSampleCount)
	{
		if(clearScreenBuffers || resizeScreenBuffers)
		{
			updateScreenBuffer = true;
		}
	}

	if(Input::GetMouseButton(MouseCode::Left) && !findNearestPrimitive)
	{
		ImGuiIO& io = ImGui::GetIO();

		// Check if mouse is not hovering above ImGui windows
		if(!io.WantCaptureMouse)
		{
			findNearestPrimitive = true;
		}
	}

	if(findNearestPrimitive)
	{
		float x = Input::GetMouseX();
		float y = screenHeight - Input::GetMouseY();

		Primitive* prim = rayTracer->SelectObject(x, y);

		if(prim != nullptr)
		{
			nearestPrimitive = prim;
		}

		findNearestPrimitive = false;
	}
}

void Renderer::Render()
{
	if(updateScreenBuffer)
	{
		//auto t1 = std::chrono::time_point_cast<std::chrono::milliseconds>((clock->now())).time_since_epoch();
		//deltaTime = (t1 - t0).count() * .001;
		//t0 = t1;

		float sampleINV = (1.0f / (float)sampleCount);
		for(int x = 0; x < screenWidth; x++)
		{
			for(int y = 0; y < screenHeight; y++)
			{
				int i = x + y * screenWidth;
				vec3 output = sampleBuffer[i];
				output = output * sampleINV;

				float g = 1.0f / 2.2f;
				output.x = pow(Clamp(output.x, 0.0f, 1.0f), g);
				output.y = pow(Clamp(output.y, 0.0f, 1.0f), g);
				output.z = pow(Clamp(output.z, 0.0f, 1.0f), g);

				screenBuffer[i] = AlbedoToRGB(output.x, output.y, output.z);
			}
		}

		if(resizeScreenBuffers)
		{
			int width, height;
			glfwGetWindowSize(window, &width, &height);
			ResizeScreenBuffers(width, height);

			resizeScreenBuffers = false;
		}

		if(clearScreenBuffers)
		{
			sampleCount = 1;
			//timeElasped = 0.0f;

			// Load in or remove new primitives to the scene //
			sceneManager->UpdateScene();

			memset(sampleBuffer, 0.0f, sizeof(vec3) * bufferSize);
			clearScreenBuffers = false;
		}

		//if(reloadSkydome)
		//{
		//	rayTracer->LoadSkydome();
		//	reloadSkydome = false;
		//}

		sampleCount++;
		if(sampleCount < targetSampleCount)
		{
			workerSystem->NotifyWorkers();

			//timeElasped += deltaTime;
			//FPSLog[frameCount % FPSLogSize] = deltaTime;
		}

		updateScreenBuffer = false;
	}

	// Copy screen-buffer data over to the Window's buffer.
	glDrawPixels(screenWidth, screenHeight, GL_RGBA, GL_UNSIGNED_BYTE, screenBuffer);
}

void Renderer::RestartSampling()
{
	clearScreenBuffers = true;
}

GLFWwindow* Renderer::GetWindow()
{
	return window;
}

void Renderer::ResizeScreenBuffers(int width, int height)
{
	screenWidth = width;
	screenHeight = height;

	bufferSize = screenWidth * screenHeight;
	delete screenBuffer;
	delete sampleBuffer;

	screenBuffer = new unsigned int[bufferSize];
	sampleBuffer = new vec3[bufferSize];

	clearScreenBuffers = true;
	workerSystem->ResizeJobTiles(screenWidth, screenHeight);

	// Update rendering side //
	rayTracer->Resize(width, height);
}

void Renderer::ClearSampleBuffer()
{
	sampleCount = 1;
	//timeElasped = 0.0f;

	// Load in or remove new primitives to the scene //
	sceneManager->UpdateScene();

	memset(sampleBuffer, 0.0f, sizeof(vec3) * bufferSize);
	clearScreenBuffers = false;
}

void Renderer::MakeScreenshot()
{
	unsigned int stride = screenWidth * sizeof(unsigned int);
	unsigned int* screenshotBuffer = new unsigned int[screenWidth * screenHeight];

	for(int x = 0; x < screenWidth; x++)
	{
		for(int y = 0; y < screenHeight; y++)
		{
			int index = x + y * screenWidth;
			unsigned int c = screenBuffer[index];
			c += (255 << 24);
			screenshotBuffer[index] = c;
		}
	}

	std::string timeStamp = std::to_string(time(NULL));
	std::string path = screenshotPath + timeStamp + ".png";

	stbi_flip_vertically_on_write(true);

	// Regular screenshot, using timestamp as name //
	stbi_write_png(path.c_str(), screenWidth, screenHeight, 4, screenshotBuffer, stride);

	// Screenshot that will be used for github readme //
	stbi_write_png(lastestScreenshotPath.c_str(), screenWidth, screenHeight, 4, screenshotBuffer, stride);

	delete[] screenshotBuffer;
}