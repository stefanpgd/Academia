#include "Renderer.h"

#include <GLFW/glfw3.h>
#include <cassert>
#include <imgui.h>
#include <stb_image_write.h>

#include "Graphics/RayTracer.h"
#include "Graphics/PostProcessor.h"

#include "Framework/Input.h"
#include "Framework/SceneManager.h"
#include "Framework/WorkerSystem.h"
#include "Utilities/Utilities.h"

Renderer::Renderer(const std::string& windowName, unsigned int screenWidth, unsigned int screenHeight) : screenWidth(screenWidth), screenHeight(screenHeight)
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

	// Intialize sub-systems //
	sceneManager = new SceneManager(screenWidth, screenHeight);
	rayTracer = new RayTracer(screenWidth, screenHeight, sceneManager->GetActiveScene());
	workerSystem = new WorkerSystem(this, screenWidth, screenHeight);
	postProcessor = new PostProcessor();
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
	// If anything in the scene gets updated ( camera, skydome, primitives )
	// We want to clear the screen, and update the content of the scene
	if(sceneManager->Update())
	{
		RestartSampling();
	}

	workerSystem->Update();

	if(sampleCount >= targetSampleCount)
	{
		// In case we reached our target frame count
		// we want to check if either something got updated or resized
		// If so, we force the screen to update again, and the path tracer restarts.
		if(clearScreenBuffers || resizeScreenBuffers)
		{
			updateScreenBuffer = true;
		}
	}

	if(Input::GetMouseButton(MouseCode::Left))
	{
		ImGuiIO& io = ImGui::GetIO();

		// Check if mouse is not hovering above ImGui windows
		if(!io.WantCaptureMouse)
		{
			float x = Input::GetMouseX();
			float y = screenHeight - Input::GetMouseY();

			Primitive* prim = rayTracer->SelectObject(x, y);
			if(prim != nullptr)
			{
				nearestPrimitive = prim;
			}
		}
	}
}

void Renderer::Render()
{
	if(updateScreenBuffer)
	{
		float sampleINV = (1.0f / (float)sampleCount);
		for(int x = 0; x < screenWidth; x++)
		{
			for(int y = 0; y < screenHeight; y++)
			{
				int i = x + y * screenWidth;
				vec3 output = sampleBuffer[i];
				output = output * sampleINV;

				screenBuffer[i] = postProcessor->PostProcess(output);
			}
		}
		sampleCount++;

		if(resizeScreenBuffers)
		{
			int width, height;
			glfwGetWindowSize(window, &width, &height);
			ResizeScreenBuffers(width, height);

			resizeScreenBuffers = false;
		}

		if(clearScreenBuffers)
		{
			ClearSampleBuffer();
		}

		if(sampleCount < targetSampleCount)
		{
			workerSystem->NotifyWorkers();
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

	glViewport(0, 0, screenWidth, screenHeight);
	bufferSize = screenWidth * screenHeight;
	delete screenBuffer;
	delete sampleBuffer;

	screenBuffer = new unsigned int[bufferSize];
	sampleBuffer = new vec3[bufferSize];

	clearScreenBuffers = true;
	workerSystem->ResizeJobTiles(screenWidth, screenHeight);
	sceneManager->GetActiveScene()->Camera->SetupVirtualPlane(screenWidth, screenHeight);
}

void Renderer::ClearSampleBuffer()
{
	sampleCount = 1;

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