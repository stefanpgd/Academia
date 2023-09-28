#include "Editor.h"
#include "Input.h"

#include <string>
#include "Framework/SceneManager.h"

Editor::Editor(GLFWwindow* window)
{
	// Setup ImGui  //
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	ImGui::StyleColorsDark();

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");
}

void Editor::Start()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

bool Editor::Update(float deltaTime)
{
	if(Input::GetKey(KeyCode::H))
	{
		// implement GetKeyDown... 
		renderEditor = !renderEditor;
	}

	if(!renderEditor)
	{
		return false;
	}

	// For now, hardcode editor elements in here.
	// in the future, having EditorElements that can be customized and just
	// added to a vector of elements that need to be renderered, might be more clean
	if(ImGui::BeginMainMenuBar())
	{
		ImGui::Text("FPS: ");
		std::string fps = std::to_string(int(1.0f / deltaTime));
		ImGui::Text(fps.c_str());

		ImGui::EndMainMenuBar();
	}

	bool sceneChanged = false;

	ImGui::Begin("Lights");

	for(int i = 0; i < activeScene->lights.size(); i++)
	{
		ImGui::PushID(i);
		Light* light = activeScene->lights[i];
		std::string name = "Light " + std::to_string(i);
		ImGui::Text(name.c_str());
		
		if(ImGui::ColorEdit3("Color", &light->Color.x))
		{
			sceneChanged = true;
		}
	}

	ImGui::End();

	return sceneChanged;
}

void Editor::Render()
{
	if (!renderEditor)
	{
		return;
	}

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Editor::SetActiveScene(Scene* scene)
{
	activeScene = scene;
}