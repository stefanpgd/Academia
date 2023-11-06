#pragma once
#include <string>
#include <vector>
#include <chrono>

// Multi-threading //
#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>

class Renderer;
class RayTracer;
class Editor;
class SceneManager;

/// <summary>
/// Current responsibilities:
/// - Handling GLFW Calls
/// - Initializing: GLFW, screenbuffers, Input, SceneManager, Editor, Ray Tracer
/// - Multithreading
/// - Destroying of resources
/// - Main loop (deltaTIme, start, update, render enc)
/// - Catching cases like resizing enc.
/// - Loading Application Settings
/// - Making a screenshot
/// </summary>

/// <summary>
/// What should the app do:
/// - Initialize systems like: Renderer, Input, Scene, Editor
/// - Destroying of resources
/// - Main Loop 
/// - Handlign GLFW Calls ( for Errors ) 
/// - Loading application settings
/// </summary>

/// <summary>
/// Renderer should:
/// - Initialize screenbuffers, Ray Tracer, Editor
/// - Initialize & Take care off: Multi-threading
/// - Link up to GLFW calls, and through it, do resizing
/// - Screenshot functionality? Maybe move to editor
/// </summary>
class App
{
public:
	App();
	~App();
	
	void Run();

private:
	void Start();
	void Update(float deltaTime);
	void Render();

	void LoadApplicationSettings();
	void SaveApplicationSettings();

private:
	bool runApp = true;
	std::string appName = "Academia";
	std::string appSettingsFile = "Settings/app.settings";

	Editor* editor;
	Renderer* renderer;
	
	// General Information //
	float timeElasped = 0.0f;
	int frameCount = 1;
	float* FPSLog;
	const int FPSLogSize = 30;

	// deltaTime //
	float deltaTime = 1.0f;
	std::chrono::high_resolution_clock* clock;
	std::chrono::milliseconds t0;

	unsigned int screenWidth;
	unsigned int screenHeight;

	friend class Editor;
};