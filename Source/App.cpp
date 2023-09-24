#include "App.h"
#include "Utilities/Utilities.h"

#include <GLFW/glfw3.h>
#include <cassert>
#include <vector>

#include "RayTracer.h"

void GLFWErrorCallback(int, const char* err_str)
{
	LOG(Log::MessageType::Error, err_str);
}

App::App()
{
	bufferSize = screenWidth * screenHeight;
	screenBuffer = new unsigned int[bufferSize];
	colorBuffer = new vec3[bufferSize];
	ClearBuffer(screenBuffer, 0x00, bufferSize);

	glfwSetErrorCallback(GLFWErrorCallback);

	if (!glfwInit())
	{
		// Check out again how to retrieve errors from GLFW
		LOG(Log::MessageType::Error, "GLFW didn't intialize properly");
		assert(false);
	}

	LOG("Succesfully initialized GLFW.");
	window = glfwCreateWindow(screenWidth, screenHeight, appName.c_str(), NULL, NULL);

	if (!window)
	{
		LOG(Log::MessageType::Error, "Failed to create a GLFW window");
		assert(false);
	}

	LOG("Succesfully created a window");
	glfwMakeContextCurrent(window);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();

	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");
}

void App::Run()
{
	RayTracer rayTracer(screenWidth, screenHeight);

	int iterations = 1;
	float pixelSizeX = (1.0f / float(screenWidth)) * 0.5f;
	float pixelSizeY = (1.0f / float(screenHeight)) * 0.5f;

	while (runApp)
	{
		if (glfwWindowShouldClose(window))
		{
			runApp = false;
		}

		frameCount++;

		glClear(GL_COLOR_BUFFER_BIT);

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		ImGui::ShowDemoWindow();
		
		for (int x = 0; x < screenWidth; x++)
		{
			for (int y = 0; y < screenHeight; y++)
			{
				int i = x + y * screenWidth;

				// determine where on the virtual screen we need to be //
				float xScale = x / float(screenWidth);
				float yScale = y / float(screenHeight);

				xScale += RandomInRange(-pixelSizeX, pixelSizeX);
				yScale += RandomInRange(-pixelSizeY, pixelSizeY);

				colorBuffer[i] += rayTracer.Trace(xScale, yScale);
				
				vec3 output = colorBuffer[i];
				output = output * (1.0f / (float)iterations);


				//float g = Dot(output, vec3(0.2126f, 0.7152f, 0.0722f));

				// Grey-Scale filter
				//float blackwhite = (output.x + output.y + output.z) * 0.33f;
				//screenBuffer[i] = AlbedoToRGB(blackwhite, blackwhite, blackwhite);
				//screenBuffer[i] = AlbedoToRGB(g, g, g);

				screenBuffer[i] = AlbedoToRGB(output.x, output.y, output.z);
			}
		}

		iterations++;

		// draw buffer into screen buffer
		glDrawPixels(screenWidth, screenHeight, GL_RGBA, GL_UNSIGNED_BYTE, screenBuffer);

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
}