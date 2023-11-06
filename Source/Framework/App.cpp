#include "App.h"
#include <GLFW/glfw3.h>

#include <cassert>
#include <vector>
#include <cmath>
#include <fstream>

#include "Renderer.h"
#include "Input.h"
#include "Editor.h"
#include "SceneManager.h"
#include "Graphics/RayTracer.h"
#include "Utilities/Utilities.h"
#include "Utilities/Timer.h"

void GLFWErrorCallback(int, const char* err_str)
{
	LOG(Log::MessageType::Error, err_str);
}

App::App()
{
	LoadApplicationSettings();

	FPSLog = new float[FPSLogSize];

	glfwSetErrorCallback(GLFWErrorCallback);

	// Start initializing custom systems  //
	sceneManager = new SceneManager(screenWidth, screenHeight);
	renderer = new Renderer(appName, screenWidth, screenHeight, sceneManager);
	
	Input::Initialize(renderer->GetWindow());
	editor = new Editor(renderer->GetWindow(), this, sceneManager);

	LOG("'Academia' has succesfully initialized!");
}

App::~App()
{
	SaveApplicationSettings();

	delete sceneManager;
	delete renderer;
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
		if(glfwWindowShouldClose(renderer->GetWindow()))
		{
			runApp = false;
		}
	}
}

void App::Start()
{
	renderer->Start();
	editor->Start();
}

void App::Update(float deltaTime)
{
	bool cameraUpdated = false;
	bool sceneUpdated = false;

	if(takeScreenshot)
	{
		//takeScreenshot = false;
		//MakeScreenshot();
	}

	if(!lockUserMovement)
	{
		// Move to SceneManager...
		//cameraUpdated = rayTracer->Update(deltaTime);
	}

	sceneUpdated = editor->Update(deltaTime);

	if(sceneUpdated || cameraUpdated)
	{
		renderer->RestartSampling();
	}
}

void App::Render()
{
	renderer->Render();
	editor->Render();
	glfwSwapBuffers(renderer->GetWindow());
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