#pragma once
#include "Math/Vec3.h"

#include <string>
#include <vector>

// Multi-threading //
#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>

struct GLFWwindow;
class RayTracer;
class SceneManager;
class Primitive;

enum class JobState
{
	ToDo,
	Processing,
	Done
};

struct JobTile
{
	JobState State = JobState::ToDo;

	unsigned int x;
	unsigned int y;
	unsigned int xMax;
	unsigned int yMax;
};

class Renderer
{
public:
	Renderer(const std::string& windowName, unsigned int screenWidth, unsigned int screenHeight);
	~Renderer();

	void Start();
	void Update();
	void Render();

	void RestartSampling();
	GLFWwindow* GetWindow();

private:
	void ResizeScreenBuffers(int width, int height);
	void ResizeJobTiles();

	void ClearSampleBuffer();

	void PathTrace();
	void MakeScreenshot();

private:
	SceneManager* sceneManager;

	// Move to editor?
	std::string screenshotPath = "Screenshots/";
	std::string lastestScreenshotPath = "Screenshots/Latest/latest.png";

	// Ray Tracing //
	RayTracer* rayTracer;
	unsigned int sampleCount = 1;
	unsigned int targetSampleCount = 100000;
	bool findNearestPrimitive = false;
	Primitive* nearestPrimitive = nullptr;
	bool doPathTracing = true;

	// Window & Back Buffers // 
	GLFWwindow* window;
	unsigned int screenWidth;
	unsigned int screenHeight;

	vec3* sampleBuffer;
	unsigned int* screenBuffer;
	unsigned int bufferSize;

	// Actions To Take //
	bool updateScreenBuffer = false;
	bool clearScreenBuffers = false;
	bool resizeScreenBuffers = false;
	bool lockUserMovement = false;
	bool reloadSkydome = false;
	bool takeScreenshot = false;

	// Multi-threading //
	int threadsAvailable;
	unsigned int tileSize = 16;
	std::thread* threads;
	std::vector<JobTile> jobTiles;
	std::atomic<int> workIndex;
	std::condition_variable iterationLock;
	std::mutex rayLock;

	friend class Editor;
};