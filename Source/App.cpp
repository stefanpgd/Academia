#include "App.h"
#include "Utilities/Utilities.h"
#include "Math/MathCommon.h"

#include <GLFW/glfw3.h>
#include <cassert>
#include <vector>

#include "Sphere.h"
#include "Plane.h"


void GLFWErrorCallback(int, const char* err_str)
{
	LOG(Log::MessageType::Error, err_str);
}

App::App()
{
	bufferSize = screenWidth * screenHeight;
	screenBuffer = new unsigned int[bufferSize];
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
	// LH system
	vec3 camera = Vec3(0.0f, 0.5f, 0.0f);
	vec3 viewDir = Vec3(0.0f, 0.0f, 1.0f);

	vec3 screenCenter = camera + viewDir;

	vec3 screenP0 = screenCenter + vec3(-0.5f, -0.5f, 0.0f);	// Bottom Left
	vec3 screenP1 = screenCenter + vec3(0.5, -0.5, 0.0f);		// Bottom Right
	vec3 screenP2 = screenCenter + vec3(-0.5f, 0.5f, 0.0f);		// Top left

	float aspect = screenWidth / float(screenHeight);
	vec3 uDir = (screenP1 - screenP0) * aspect;
	vec3 vDir = screenP2 - screenP0;

	int frameCount = 0;

	std::vector<Primitive*> primitives;
	primitives.push_back(new Sphere(vec3(1.5f, 0.5f, 4.0f), 0.5f));
	primitives.push_back(new Sphere(vec3(2.5f, 0.15f, 4.0f), 0.15f, vec3(0.4f, 1.0f, 0.6f)));
	primitives.push_back(new Plane(vec3(0.0f, 0.0f, 5.0f), Normalize(vec3(0.0f, 1.0f, 0.0f))));

	const float maxDepth = 10000.0f;

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
				// determine where on the virtual screen we need to be //
				float xScale = x / float(screenWidth);
				float yScale = y / float(screenHeight);

				// Using our screen vectors, determine our ray direction //
				vec3 screenPoint = screenP0;
				screenPoint += uDir * xScale;
				screenPoint += vDir * yScale;

				vec3 rayDir = screenPoint - camera;
				rayDir.Normalize();

				Ray ray = Ray(camera, rayDir);

				screenBuffer[x + y * screenWidth] = 0x00;

				HitRecord record;
				record.t = maxDepth;

				HitRecord tempRecord;

				for (Primitive* prim : primitives)
				{
					prim->Intersect(ray, tempRecord);

					if (tempRecord.t > 0.0f && tempRecord.t < record.t)
					{
						record = tempRecord;
					}
				}

				if (record.t != maxDepth)
				{
					vec3 lightP = vec3(0.0f, 2.0f, 0.0f);
					vec3 lightD = Normalize(lightP - record.HitPoint);

					vec3 LightDIn = record.HitPoint - lightP;
					LightDIn.Normalize();

					vec3 hitToEye = camera - record.HitPoint;
					hitToEye.Normalize();
					
					float c = max(Dot(lightD, record.Normal), 0.0);
					c += 0.075f;
					c = min(c, 1.0f);

					vec3 lightC = vec3(1.0f);
					vec3 col = record.Primitive->Color;
					vec3 outputC = vec3(lightC.x * col.x, lightC.y * col.y, lightC.z * col.z);
					outputC = outputC * c;
					screenBuffer[x + y * screenWidth] = (int(outputC.x * 255.0f) << 16) | (int(outputC.y * 255.0f) << 8) | int(outputC.z * 255.0f);
				}
			}
		}

		// draw buffer into screen buffer
		glDrawPixels(screenWidth, screenHeight, GL_RGBA, GL_UNSIGNED_BYTE, screenBuffer);

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
}