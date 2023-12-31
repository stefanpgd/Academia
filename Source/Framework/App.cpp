#include "App.h"
#include <GLFW/glfw3.h>

#include <cassert>
#include <fstream>

#include "Renderer.h"
#include "Input.h"
#include "Editor.h"
#include "SceneManager.h"
#include "Utilities/Utilities.h"

void GLFWErrorCallback(int, const char* err_str)
{
	LOG(Log::MessageType::Error, err_str);
}

App::App()
{
	LoadApplicationSettings();
	glfwSetErrorCallback(GLFWErrorCallback);

	// Start initializing custom systems  //
	renderer = new Renderer(appName, screenWidth, screenHeight);
	Input::Initialize(renderer->GetWindow());
	editor = new Editor(renderer->GetWindow(), this);

	LOG("'Academia' has succesfully initialized!");
}

App::~App()
{
	SaveApplicationSettings();

	delete renderer;
}

void App::Run()
{
	while(runApp)
	{
		Start();
		Update();
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

void App::Update()
{
	renderer->Update();
	editor->Update();
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

	int width, height;
	glfwGetWindowSize(renderer->GetWindow(), &width, &height);

	appSettings << width << "\n";
	appSettings << height << "\n";

	LOG("Application settings succesfully saved!");
}