#include "Editor.h"
#include "Input.h"

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

void Editor::Update()
{
	if (Input::GetKey(KeyCode::H))
	{
		// implement GetKeyDown... 
		renderEditor = !renderEditor;
	}

	if (!renderEditor)
	{
		return;
	}

	// For now, hardcode editor elements in here.
	// in the future, having EditorElements that can be customized and just
	// added to a vector of elements that need to be renderered, might be more clean

	if (ImGui::BeginMainMenuBar())
	{
		ImGui::EndMainMenuBar();
	}
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