#include "App.h"
#include "Utilities/Utilities.h"

#include <cassert>
#include <vector>
#include <cmath>
#include <fstream>
#include <stb_image_write.h>

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
	LoadApplicationSettings();

	FPSLog = new float[FPSLogSize];
	bufferSize = screenWidth * screenHeight;
	screenBuffer = new unsigned int[bufferSize];
	colorBuffer = new vec3[bufferSize];
	ClearBuffer(screenBuffer, 0x00, bufferSize);

	glfwSetErrorCallback(GLFWErrorCallback);

	if(!glfwInit())
	{
		// Check out again how to retrieve errors from GLFW
		LOG(Log::MessageType::Error, "GLFW didn't intialize properly");
		assert(false);
	}

	LOG("Succesfully initialized GLFW.");
	window = glfwCreateWindow(screenWidth, screenHeight, appName.c_str(), NULL, NULL);

	if(!window)
	{
		LOG(Log::MessageType::Error, "Failed to create a GLFW window!");
		assert(false);
	}

	LOG("Succesfully created a window.");
	glfwMakeContextCurrent(window);
	glfwSetWindowSizeCallback(window, GLFWWindowResize);

	// Start initializing custom systems  //
	Input::Initialize(window);

	sceneManager = new SceneManager(screenWidth, screenHeight);
	editor = new Editor(window, this, sceneManager);
	editor->SetActiveScene(sceneManager->GetActiveScene());

	rayTracer = new RayTracer(screenWidth, screenHeight, sceneManager->GetActiveScene());

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

	LOG("'Academia' has succesfully initialized!");
}

App::~App()
{
	SaveApplicationSettings();

	delete sceneManager;

	glfwDestroyWindow(window);
}

void App::Run()
{
	clock = new std::chrono::high_resolution_clock();
	t0 = std::chrono::time_point_cast<std::chrono::milliseconds>((clock->now())).time_since_epoch();

	while(runApp)
	{
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

	if(takeScreenshot)
	{
		takeScreenshot = false;
		MakeScreenshot();
	}

	if(!lockUserMovement)
	{
		cameraUpdated = rayTracer->Update(deltaTime);
	}

	sceneUpdated = editor->Update(deltaTime);

	if(sceneUpdated || cameraUpdated)
	{
		clearScreenBuffers = true;
	}

	// In case we reached our target frame count
	// we want to check if either something got updated or resized
	// If so, we force the screen to update again, and the path tracer restarts.
	if(frameCount >= targetSampleCount)
	{
		if(clearScreenBuffers || resizeScreenBuffers)
		{
			updateScreenBuffer = true;
		}
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

void App::Render()
{
	if(updateScreenBuffer)
	{
		auto t1 = std::chrono::time_point_cast<std::chrono::milliseconds>((clock->now())).time_since_epoch();
		deltaTime = (t1 - t0).count() * .001;
		t0 = t1;

		for(int x = 0; x < screenWidth; x++)
		{
			for(int y = 0; y < screenHeight; y++)
			{
				int i = x + y * screenWidth;
				vec3 output = colorBuffer[i];
				output = output * (1.0f / (float)frameCount);

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
			ResizeBuffers(width, height);

			resizeScreenBuffers = false;
		}

		if(clearScreenBuffers)
		{
			frameCount = 1;
			timeElasped = 0.0f;

			// Load in or remove new primitives to the scene //
			sceneManager->UpdateScene();

			memset(colorBuffer, 0.0f, sizeof(vec3) * bufferSize);
			clearScreenBuffers = false;
		}

		if(reloadSkydome)
		{
			rayTracer->LoadSkydome();
			reloadSkydome = false;
		}

		for(unsigned int i = 0; i < jobTiles.size(); i++)
		{
			jobTiles[i].State = TileState::ToDo;
		}

		frameCount++;
		if(frameCount < targetSampleCount)
		{
			// Notify the workers again //
			workIndex.store(jobTiles.size() - 1);
			iterationLock.notify_all();

			timeElasped += deltaTime;
			FPSLog[frameCount % FPSLogSize] = deltaTime;
		}

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

void App::LoadApplicationSettings()
{
	LOG("Loading Application Settings...");

	std::string line;
	std::ifstream appSettings(appSettingsFile);

	if(appSettings.is_open())
	{
		// TO-DO: Replace with Json in the future
		LOG("Found application settings, loading previously used settings.");

		// Screen Width & Screen Height // 
		std::getline(appSettings, line);
		screenWidth = std::stof(line);

		std::getline(appSettings, line);
		screenHeight = std::stof(line);

		LOG("Application settings succesfully loaded!");
	}
	else
	{
		LOG(Log::MessageType::Debug, "No application settings found, using default settings.");
	}
}

void App::SaveApplicationSettings()
{
	LOG("Saving application settings...");
	std::ofstream appSettings;
	appSettings.open(appSettingsFile, std::fstream::out);
	appSettings.clear();

	appSettings << screenWidth << "\n";
	appSettings << screenHeight << "\n";

	LOG("Application settings succesfully saved!");
}

void App::MakeScreenshot()
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