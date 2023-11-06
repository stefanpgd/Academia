#pragma once
#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <imgui_stdlib.h>

#include <array>
#include <string>
#include <vector>

#include "Math/Vec3.h"

class GLFWwindow;
class App;
class SceneManager;
class Primitive;

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

private:
	// Editor Windows //
	void MenuBar();

	void SceneSettings();
	void PathTracerSettings();
	void SkydomeSettings();
	void CameraSettings();

	void PrimitiveSelection();
	void PrimitiveHierarchy();
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
		"Sphere        ",
		"Plane         ",
		"Plane Infinite",
		"Triangle      "
	};
	unsigned int selectedPrimitiveType = 0;

	vec3 primPosition;
	vec3 primNormal;
	float primScale = 0.5f;

	struct ImFont* baseFont;
	struct ImFont* boldFont;

	std::vector<std::string> exrFilePaths;
	std::string placeholderName;

	Primitive* selectedPrimitive = nullptr;

	// Windows States //
	bool showSceneHierarchy = true;
	bool showPrimitiveSelection = true;
	bool showSceneSettings = true;
	bool showPrimitiveCreation = true;
};