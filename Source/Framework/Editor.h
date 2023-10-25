#pragma once
#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <array>
#include <string>
#include <vector>

#include "Math/Vec3.h"

class GLFWwindow;
class App;
class SceneManager;

struct Scene;

/// <summary>
/// Responsible for managing everything related to the Editor/UI.
/// Which includes initializing ImGui
/// </summary>
class Editor
{
public:
	Editor(GLFWwindow* window, App* app, SceneManager* sceneManager);

	void Start();
	bool Update(float deltaTime);
	void Render();

	void SetActiveScene(Scene* scene);

private:
	// Editor Windows //
	void MenuBar();

	void PathTracerSettings();
	void SkydomeSettings();
	void CameraSettings();

	void PrimitiveHierachy();
	void PrimitiveCreation();

	void LoadEXRFilePaths();
	void ImGuiStyleSettings();

private:
	bool sceneUpdated = false;
	bool renderEditor = true;

	App* app;
	Scene* activeScene;
	SceneManager* sceneManager;

	// Primitive Creation //
	std::array<const char*, 4> primitiveNames{
		"Sphere",
		"Plane",
		"PlaneInfinite",
		"Triangle"
	};
	unsigned int selectedPrimitive = 0;

	vec3 primPosition;
	vec3 primNormal;
	float primScale = 0.5f;

	struct ImFont* baseFont;
	struct ImFont* boldFont;

	// Windows //
	bool showPathTracerSettings = true;
	bool showSkydomeSettings = true;
	bool showCameraSettings = true;

	std::vector<std::string> exrFilePaths;
};