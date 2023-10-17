#include "App.h"
#include "Utilities/Utilities.h"

#include <cassert>
#include <vector>
#include <chrono>
#include <cmath>

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

	LOG("Retrieving thread count...");
	threadsAvailable = std::thread::hardware_concurrency();
	threads = new std::thread[threadsAvailable];
	LOG("There are: '" + std::to_string(threadsAvailable) + "' threads available.");

	ResizeJobTiles();
	workIndex.store(jobTiles.size() - 1);

	for(int i = 0; i < threadsAvailable; i++)
	{
		threads[i] = std::thread([this] {TraceTile(); });
	}
	LOG("Multi-threading succesfully started.");
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
		resizeScreenBuffers = true;
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
		clearScreenBuffers = true;
	}

	if(workIndex < 0)
	{
		// All jobs have been picked up, check if they are done as well
		// if so, that means an trace iteration has been completed, and we can
		// update the screen buffer

		bool iterationDone = true;

		for(unsigned int i = 0; i < jobTiles.size(); i++)
		{
			if(jobTiles[i].State != TileState::Done)
			{
				iterationDone = false;
			}
		}

		if(iterationDone)
		{
			updateScreenBuffer = true;
		}
	}

	// Process input
	// Process editor & Scene stuff...
	// Update components like camera

	// Potentially reset buffers like colorBuffer incase scene got updated
}

void App::Render()
{
	if(updateScreenBuffer)
	{
		for(int x = 0; x < screenWidth; x++)
		{
			for(int y = 0; y < screenHeight; y++)
			{
				int i = x + y * screenWidth;
				vec3 output = colorBuffer[i];
				output = output * (1.0f / (float)frameCount);

				screenBuffer[i] = AlbedoToRGB(output.x, output.y, output.z);
			}
		}

		// Notify the workers again //
		for(unsigned int i = 0; i < jobTiles.size(); i++)
		{
			jobTiles[i].State = TileState::ToDo;
		}

		if(resizeScreenBuffers)
		{
			int width, height;
			glfwGetWindowSize(window, &width, &height);
			ResizeBuffers(width, height);
			resizeScreenBuffers = false;
		}

		if(clearScreenBuffers && frameCount > 6)
		{
			ClearScreenbuffers();
			clearScreenBuffers = false;
		}

		workIndex.store(jobTiles.size() - 1);
		iterationLock.notify_all();

		frameCount++;
		updateScreenBuffer = false;
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
	
	clearScreenBuffers = true;
	ResizeJobTiles();

	// Update rendering side //
	rayTracer->Resize(width, height);
}

void App::ClearScreenbuffers()
{
	frameCount = 1;

	ClearBuffer(screenBuffer, 0x00, bufferSize);
	memset(colorBuffer, 0.0f, sizeof(vec3) * bufferSize);
}

void App::TraceTile()
{
	while(runApp)
	{
		// Retrieve job tile index
		int index = workIndex.fetch_sub(1);

		// All jobs available are either in process or have been completed
		// Wait until you receive a signal to continue again.
		if(index < 0)
		{
			std::unique_lock<std::mutex> lock(rayLock);
			iterationLock.wait(lock);
			continue;
		}

		JobTile& tile = jobTiles[index];

		if(tile.State != TileState::ToDo)
		{
			continue;
		}

		tile.State = TileState::Processing;

		for(int x = tile.x; x < tile.xMax; x++)
		{
			for(int y = tile.y; y < tile.yMax; y++)
			{
				int i = x + y * screenWidth;
				colorBuffer[i] += rayTracer->Trace(x, y);
			}
		}

		tile.State = TileState::Done;
	}
}

void App::ResizeJobTiles()
{
	jobTiles.clear();

	int horizontalTiles = screenWidth / tileSize;
	int verticalTiles = screenHeight / tileSize;

	for(unsigned int y = 0; y < verticalTiles; y++)
	{
		for(unsigned int x = 0; x < horizontalTiles; x++)
		{
			JobTile tile;
			tile.x = x * tileSize;
			tile.y = y * tileSize;
			tile.xMax = x * tileSize + tileSize;
			tile.yMax = y * tileSize + tileSize;

			jobTiles.push_back(tile);
		}
	}
}