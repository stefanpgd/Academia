#pragma once
#include <string>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "Math/Vec3.h"

class GLFWwindow;

class App
{
public:
	App();
	
	void Run();

private:
	int frameCount = 0;

	bool runApp = true;
	std::string appName = "Academia";

	GLFWwindow* window;

	// In future, save this to a text file and load in last settings.
	unsigned int screenWidth = 720;
	unsigned int screenHeight = 720;
	unsigned int bufferSize;

	vec3* colorBuffer;
	unsigned int* screenBuffer;
};