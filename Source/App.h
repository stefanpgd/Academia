#pragma once
#include <string>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "Math/Vec3.h"

class GLFWwindow;
class RayTracer;

class App
{
public:
	App();
	~App();
	
	
	void Run();

private:
	void Start();
	void Update();
	void Render();

private:
	bool runApp = true;
	std::string appName = "Academia";
	GLFWwindow* window;

	// Ray Tracer //
	RayTracer* rayTracer;
	int frameCount = 1;

	// In future, save this to a text file and load in last settings.
	unsigned int screenWidth = 720;
	unsigned int screenHeight = 720;

	// Screen Buffers // 
	vec3* colorBuffer;
	unsigned int* screenBuffer;
	unsigned int bufferSize;
};