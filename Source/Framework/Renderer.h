#pragma once
#include "Math/Vec3.h"
#include <string>
#include <chrono>

struct GLFWwindow;

class Primitive;
class RayTracer;
class SceneManager;
class WorkerSystem;
class PostProcessor;

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
	void ClearSampleBuffer();

	void MakeScreenshot();

private:
	RayTracer* rayTracer;
	WorkerSystem* workerSystem;
	SceneManager* sceneManager;
	PostProcessor* postProcessor;

	// Move t
	std::string screenshotPath = "Screenshots/";
	std::string lastestScreenshotPath = "Screenshots/Latest/latest.png";

	// Ray Tracing //
	int sampleCount = 1;
	int targetSampleCount = 100000;
	Primitive* nearestPrimitive = nullptr;

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
	bool takeScreenshot = false;

	// Time //
	float deltaTime = 1.0f;
	std::chrono::high_resolution_clock* clock;
	std::chrono::milliseconds t0;
	float* FPSLog;
	float renderTime = 0.0f;
	const int FPSLogSize = 30;

	friend class Editor;
	friend class WorkerSystem;
};