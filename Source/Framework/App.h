#pragma once
#include <string>
#include <vector>
#include <chrono>
#include <GLFW/glfw3.h>
#include "Math/Vec3.h"

// Multi-threading //
#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>

class GLFWwindow;
class RayTracer;
class Editor;
class SceneManager;
class Primitive;

enum class TileState
{
	ToDo,
	Processing,
	Done
};

struct JobTile
{
	TileState State = TileState::ToDo;

	unsigned int x;
	unsigned int y;
	unsigned int xMax;
	unsigned int yMax;
};

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

	void ResizeBuffers(int width, int height);
	void ClearScreenbuffers();

	void TraceTile();
	void ResizeJobTiles();

	void LoadApplicationSettings();
	void SaveApplicationSettings();

private:
	bool runApp = true;
	std::string appName = "Academia";
	std::string appSettingsFile = "Settings/app.settings";
	GLFWwindow* window;

	Editor* editor;
	SceneManager* sceneManager;
	
	// General Information //
	float timeElasped = 0.0f;
	int frameCount = 1;
	float* FPSLog;
	const int FPSLogSize = 30;

	// Ray Tracer //
	RayTracer* rayTracer;
	bool findNearestPrimitive = false;
	Primitive* nearestPrimitive = nullptr;

	// Multi-threading //
	int threadsAvailable;
	unsigned int tileSize = 16;
	std::thread* threads;
	std::vector<JobTile> jobTiles;
	std::atomic<int> workIndex;
	std::condition_variable iterationLock;
	std::mutex rayLock;

	bool updateScreenBuffer = false;
	bool clearScreenBuffers = false;
	bool resizeScreenBuffers = false;

	// In future, save this to a text file and load in last settings.
	unsigned int screenWidth = 720;
	unsigned int screenHeight = 720;

	// Screen Buffers // 
	vec3* colorBuffer;
	unsigned int* screenBuffer;
	unsigned int bufferSize;

	// deltaTime //
	float deltaTime = 0.0f;
	std::chrono::high_resolution_clock* clock;
	std::chrono::milliseconds t0;

	friend class Editor;
};