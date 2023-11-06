#pragma once
#include "Math/Vec3.h"
#include <string>

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
	WorkerSystem* workerSystem;
	SceneManager* sceneManager;
	RayTracer* rayTracer;
	PostProcessor* postProcessor;

	// Move to editor?
	std::string screenshotPath = "Screenshots/";
	std::string lastestScreenshotPath = "Screenshots/Latest/latest.png";

	// Ray Tracing //
	unsigned int sampleCount = 1;
	unsigned int targetSampleCount = 100000;
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
	bool lockUserMovement = false;
	bool reloadSkydome = false;
	bool takeScreenshot = false;

	friend class Editor;
	friend class WorkerSystem;
};