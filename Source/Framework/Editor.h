#pragma once
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

class GLFWwindow;
struct Scene;

/// <summary>
/// Responsible for managing everything related to the Editor/UI.
/// Which includes initializing ImGui
/// </summary>
class Editor
{
public:
	Editor(GLFWwindow* window);

	void Start();
	bool Update(float deltaTime);
	void Render();

	void SetActiveScene(Scene* scene);

private:
	void PrimitiveHierachy();
	void PrimitiveCreation();

private:
	bool sceneUpdated = false;
	bool renderEditor = true;
	Scene* activeScene;

	friend class App;
};