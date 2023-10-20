#include "Editor.h"
#include "Input.h"

#include <string>
#include "Framework/SceneManager.h"

#include "Graphics/Sphere.h"

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

	ImGui::Begin("Primitives");
	for(int i = 0; i < activeScene->primitives.size(); i++)
	{
		ImGui::PushID(i);
		ImGui::Separator();
		Primitive* primitive = activeScene->primitives[i];
		std::string name = "Primitive " + std::to_string(i);
		ImGui::Text(name.c_str());

		if(ImGui::DragFloat3("Position", &primitive->Position.x, 0.01f)) { sceneChanged = true; }

		// Primitive specific editor options // 
		switch(primitive->Type)
		{
		case PrimitiveType::Sphere:
			Sphere* sphere = dynamic_cast<Sphere*>(primitive);

			if(ImGui::InputFloat("Scale", &sphere->Radius, 0.1f, 0.5f)) 
			{ 
				sphere->Radius2 = sphere->Radius * sphere->Radius;
				sceneChanged = true; 
			}
			break;
		}

		// Material Properties // 
		Material* material = &activeScene->primitives[i]->Material;

		ImGui::Text("Material Properties:");

		// In future, use a dropdown menu to define between:
		// 1. Opaque
		// 2. Dielectric
		// 3. Emissive
		if(material->isDielectric) 
		{
			if(ImGui::ColorEdit3("Color", &material->Color.x, 0.01f)) { sceneChanged = true; }
			if(ImGui::DragFloat("IoR", &material->IoR, 0.002f, 1.0f, 3.0f)) { sceneChanged = true; }

			if(ImGui::Checkbox("Is Dielectric", &material->isDielectric)) { sceneChanged = true; }
		}
		else if(material->isEmissive)
		{
			if(ImGui::ColorEdit3("Color", &material->Color.x, 0.01f)) { sceneChanged = true; }
			if(ImGui::DragFloat("Emissive Strength", &material->EmissiveStrength, 0.02f, 0.0f, 100.0f)) { sceneChanged = true; }

			if(ImGui::Checkbox("Is Emissive", &material->isEmissive)) { sceneChanged = true; }
		}
		else // then material is Opaque model
		{
			if(ImGui::ColorEdit3("Color", &material->Color.x, 0.01f)) { sceneChanged = true; }
			if(ImGui::DragFloat("Specularity", &material->Specularity, 0.002f, 0.0f, 1.0f)) { sceneChanged = true; }
			if(ImGui::DragFloat("Roughness", &material->Roughness, 0.002f, 0.0f, 1.0f)) { sceneChanged = true; }
			if(ImGui::DragFloat("Metalness", &material->Metalness, 0.002f, 0.0f, 1.0f)) { sceneChanged = true; }
			if(ImGui::DragFloat("IoR", &material->IoR, 0.002f, 1.0f, 3.0f)) { sceneChanged = true; }

			if(ImGui::Checkbox("Is Dielectric", &material->isDielectric)) { sceneChanged = true; }
			if(ImGui::Checkbox("Is Emissive", &material->isEmissive)) { sceneChanged = true; }
		}

		ImGui::Separator();
		ImGui::PopID();
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