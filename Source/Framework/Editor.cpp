#include "Editor.h"
#include <string>

#include "Framework/App.h"
#include "Framework/Input.h"
#include "Framework/SceneManager.h"

#include "Graphics/RayTracer.h"
#include "Graphics/Sphere.h"
#include "Graphics/PlaneInfinite.h"

Editor::Editor(GLFWwindow* window, App* app, SceneManager* sceneManager) : app(app), sceneManager(sceneManager)
{
	// Setup ImGui  //
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	ImGui::StyleColorsDark();

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 460");

	ImGuiStyleSettings();
}

void Editor::Start()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

bool Editor::Update(float deltaTime)
{
	sceneUpdated = false;

	if(Input::GetKey(KeyCode::H))
	{
		// implement GetKeyDown...
		renderEditor = !renderEditor;
	}

	if(!renderEditor)
	{
		return false;
	}

	ImGui::ShowDemoWindow();

	MenuBar();
	PrimitiveHierachy();
	PrimitiveCreation();

	PathTracerSettings();
	SkydomeSettings();
	CameraSettings();

	return sceneUpdated;
}

void Editor::Render()
{
	if(!renderEditor)
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

void Editor::MenuBar()
{
	if(ImGui::BeginMainMenuBar())
	{
		// Window Selection //
		if(ImGui::BeginMenu("Windows"))
		{
			if(ImGui::MenuItem("Path Tracer Settings", NULL, &showPathTracerSettings)) {} 
			if(ImGui::MenuItem("Skydome Settings", NULL, &showSkydomeSettings)) {}
			if(ImGui::MenuItem("Camera Settings", NULL, &showCameraSettings)) {}

			ImGui::EndMenu();
		}

		// FPS // 
		ImGui::PushFont(boldFont);
		ImGui::Text("FPS:");
		ImGui::PopFont();

		std::string fps = std::to_string(int(1.0f / app->deltaTime));
		ImGui::Text(fps.c_str());

		ImGui::Separator();

		// Average FPS //
		float average = 0.0f;
		for(int i = 0; i < app->FPSLogSize; i++)
		{
			average += app->FPSLog[i];
		}
		average /= app->FPSLogSize;

		ImGui::PushFont(boldFont);
		ImGui::Text("Average FPS:");
		ImGui::PopFont();

		std::string averageFPS = std::to_string(int(1.0f / average));
		ImGui::Text(averageFPS.c_str());

		ImGui::Separator();

		// Samples Taken //
		ImGui::PushFont(boldFont);
		ImGui::Text("Samples Taken:");
		ImGui::PopFont();

		std::string samplesTaken = std::to_string(app->frameCount);
		ImGui::Text(samplesTaken.c_str());

		ImGui::Separator();

		// Time Elasped // 
		ImGui::PushFont(boldFont);
		ImGui::Text("Time Elapsed:");
		ImGui::PopFont();

		int min = int(app->timeElasped / 60.0f);
		std::string minutes = std::to_string(min);
		if(min < 10)
		{
			minutes = "0" + minutes;
		}

		int sec = int(app->timeElasped) % 60;
		std::string seconds = std::to_string(sec);
		if(sec < 10)
		{
			seconds = "0" + seconds;
		}

		float milli = app->timeElasped - int(app->timeElasped);
		std::string milliseconds = std::to_string(int(milli * 100.0f));
		if(milli < 0.1f)
		{
			milliseconds = "0" + milliseconds;
		}

		std::string time = minutes + ":" + seconds + ":" + milliseconds;
		ImGui::Text(time.c_str());

		ImGui::Separator();

		ImGui::EndMainMenuBar();
	}
}

void Editor::PathTracerSettings()
{
	if(!showPathTracerSettings)
	{
		return;
	}

	// Window Positioning & Flags //
	const int width = 350;
	const int height = 138;
	const int x = app->screenWidth - width;
	const int y = 18;

	ImGui::SetNextWindowPos(ImVec2(x, y));
	ImGui::SetNextWindowSize(ImVec2(width, height));

	ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse;

	ImGui::PushFont(boldFont);
	ImGui::Begin("Path Tracing Settings", nullptr, flags);
	ImGui::PushFont(baseFont);

	ImGui::Columns(2);

	ImGui::Separator();
	ImGui::AlignTextToFramePadding();
	ImGui::Text("Max Luminance Per Frame");
	ImGui::NextColumn();
	if(ImGui::DragFloat("##1", &app->rayTracer->maxLuminance, 0.1f, 0.0f, 1000.0f)) { sceneUpdated = true; }
	ImGui::NextColumn();

	ImGui::Separator();
	ImGui::AlignTextToFramePadding();
	ImGui::Text("Ray Depth");
	ImGui::NextColumn();
	if(ImGui::DragInt("##2", &app->rayTracer->maxRayDepth, 0.02f, 1, 100)) { sceneUpdated = true; }
	ImGui::NextColumn();

	ImGui::Separator();
	ImGui::AlignTextToFramePadding();
	ImGui::Text("Max Ray T");
	ImGui::NextColumn();
	if(ImGui::DragFloat("##3", &app->rayTracer->maxT, 1.0f, 0.0f, 100000)) { sceneUpdated = true; }
	ImGui::NextColumn();

	ImGui::Separator();
	ImGui::AlignTextToFramePadding();
	ImGui::Text("Use Skydome");
	ImGui::NextColumn();
	if(ImGui::Checkbox("##4", &app->rayTracer->useSkydomeTexture)) { sceneUpdated = true; }

	ImGui::Columns(1);
	ImGui::Separator();

	ImGui::PopFont();
	ImGui::End();
	ImGui::PopFont();
}

void Editor::SkydomeSettings()
{
	if(!showSkydomeSettings)
	{
		return;
	}

	// Window Positioning & Flags //
	const int width = 350;
	const int height = 220;
	const int x = app->screenWidth - width;
	const int y = 155;

	ImGui::SetNextWindowPos(ImVec2(x, y));
	ImGui::SetNextWindowSize(ImVec2(width, height));

	ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse;

	ImGui::PushFont(boldFont);
	ImGui::Begin("Skydome Settings", nullptr, flags);
	ImGui::PushFont(baseFont);

	ImGui::PushFont(boldFont);
	ImGui::SeparatorText("Skydome");
	ImGui::PushFont(baseFont);

	ImGui::Columns(2);

	ImGui::Separator();
	ImGui::AlignTextToFramePadding();
	ImGui::Text("Skydome Emission");
	ImGui::NextColumn();
	if(ImGui::DragFloat("##1", &app->rayTracer->scene->SkyDomeEmission, 0.01f)) { sceneUpdated = true; }
	ImGui::NextColumn();

	ImGui::Separator();
	ImGui::AlignTextToFramePadding();
	ImGui::Text("Skydome Background Strength");
	ImGui::NextColumn();
	if(ImGui::DragFloat("##2", &app->rayTracer->scene->SkyDomeBackgroundStrength, 0.01f, 0.0f, 10.0f)) { sceneUpdated = true; }
	ImGui::NextColumn();

	ImGui::Separator();
	ImGui::AlignTextToFramePadding();
	ImGui::Text("Skydome Orientation");
	ImGui::NextColumn();
	if(ImGui::SliderFloat("##3", &app->rayTracer->scene->SkydomeOrientation, 0.0f, 1.0f)) { sceneUpdated = true; }
	ImGui::NextColumn();

	ImGui::Columns(1);
	ImGui::Separator();

	ImGui::PushFont(boldFont);
	ImGui::SeparatorText("Sky Color");
	ImGui::PushFont(baseFont);

	ImGui::Columns(2);

	ImGui::Separator();
	ImGui::AlignTextToFramePadding();
	ImGui::Text("Sky Color - Up");
	ImGui::NextColumn();
	if(ImGui::ColorEdit3("##4", &app->rayTracer->skyColorB.x)) { sceneUpdated = true; }
	ImGui::NextColumn();

	ImGui::Separator();
	ImGui::AlignTextToFramePadding();
	ImGui::Text("Sky Color - Down");
	ImGui::NextColumn();
	if(ImGui::ColorEdit3("##5", &app->rayTracer->skyColorA.x)) { sceneUpdated = true; }
	ImGui::NextColumn();

	ImGui::Columns(1);
	ImGui::Separator();

	ImGui::PopFont();
	ImGui::End();
	ImGui::PopFont();
}

void Editor::CameraSettings()
{
	if(!showCameraSettings)
	{
		return;
	}

	// Window Positioning & Flags //
	const int width = 350;
	const int height = 220;
	const int x = app->screenWidth - width;
	const int y = 375;

	ImGui::SetNextWindowPos(ImVec2(x, y));
	ImGui::SetNextWindowSize(ImVec2(width, height));

	ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse;

	ImGui::PushFont(boldFont);
	ImGui::Begin("Camera Settings", nullptr, flags);
	ImGui::PushFont(baseFont);

	ImGui::PopFont();
	ImGui::End();
	ImGui::PopFont();
}

void Editor::PrimitiveHierachy()
{
	ImGui::Begin("Primitives");
	for(int i = 0; i < activeScene->primitives.size(); i++)
	{
		ImGui::PushID(i);
		ImGui::Separator();
		Primitive* primitive = activeScene->primitives[i];
		std::string name = "Primitive " + std::to_string(i);
		ImGui::Text(name.c_str());

		if(ImGui::DragFloat3("Position", &primitive->Position.x, 0.01f)) { sceneUpdated = true; }

		// Primitive specific editor options // 
		switch(primitive->Type)
		{
		case PrimitiveType::Sphere:
			Sphere* sphere = dynamic_cast<Sphere*>(primitive);

			if(ImGui::InputFloat("Scale", &sphere->Radius, 0.1f, 0.5f))
			{
				sphere->Radius2 = sphere->Radius * sphere->Radius;
				sceneUpdated = true;
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
			if(ImGui::ColorEdit3("Color", &material->Color.x, 0.01f)) { sceneUpdated = true; }
			if(ImGui::DragFloat("IoR", &material->IoR, 0.002f, 1.0f, 3.0f)) { sceneUpdated = true; }
			if(ImGui::DragFloat("Density", &material->Density, 0.01, 0.0f, 100.0f)) { sceneUpdated = true; }

			if(ImGui::Checkbox("Is Dielectric", &material->isDielectric)) { sceneUpdated = true; }
		}
		else if(material->isEmissive)
		{
			if(ImGui::ColorEdit3("Color", &material->Color.x, 0.01f)) { sceneUpdated = true; }
			if(ImGui::DragFloat("Emissive Strength", &material->EmissiveStrength, 0.02f, 0.0f, 100.0f)) { sceneUpdated = true; }

			if(ImGui::Checkbox("Is Emissive", &material->isEmissive)) { sceneUpdated = true; }
		}
		else // then material is Opaque model
		{
			if(ImGui::ColorEdit3("Color", &material->Color.x, 0.01f)) { sceneUpdated = true; }
			if(ImGui::DragFloat("Specularity", &material->Specularity, 0.002f, 0.0f, 1.0f)) { sceneUpdated = true; }
			if(ImGui::DragFloat("Roughness", &material->Roughness, 0.002f, 0.0f, 1.0f)) { sceneUpdated = true; }
			if(ImGui::DragFloat("Metalness", &material->Metalness, 0.002f, 0.0f, 1.0f)) { sceneUpdated = true; }
			if(ImGui::DragFloat("IoR", &material->IoR, 0.002f, 1.0f, 3.0f)) { sceneUpdated = true; }

			if(ImGui::Checkbox("Is Dielectric", &material->isDielectric)) { sceneUpdated = true; }
			if(ImGui::Checkbox("Is Emissive", &material->isEmissive)) { sceneUpdated = true; }
		}

		if(ImGui::Button("Delete Primitive", ImVec2(50, 20)))
		{
			primitive->MarkedForDelete = true;
			sceneUpdated = true;
		}

		ImGui::Separator();
		ImGui::PopID();
	}
	ImGui::End();
}

void Editor::PrimitiveCreation()
{
	ImGui::Begin("Primitive Creation");

	if(ImGui::BeginCombo("Primitive Type", primitiveNames[selectedPrimitive]))
	{
		for(int i = 0; i < primitiveNames.size(); i++)
		{
			bool isSelected = primitiveNames[i] == primitiveNames[selectedPrimitive];

			if(ImGui::Selectable(primitiveNames[i], isSelected))
			{
				selectedPrimitive = i;
			}

			if(isSelected)
			{
				ImGui::SetItemDefaultFocus();
			}
		}

		ImGui::EndCombo();
	}

	PrimitiveType type = PrimitiveType(selectedPrimitive);
	switch(type)
	{
	case PrimitiveType::Sphere:
		ImGui::DragFloat3("Position", &primPosition.x, 0.025f);
		ImGui::DragFloat("Scale", &primScale, 0.05f);

		if(ImGui::Button("Add Sphere"))
		{
			Sphere* sphere = new Sphere(primPosition, primScale);
			sceneManager->AddPrimitiveToScene(sphere);
			sceneUpdated = true;
		}
		break;

	case PrimitiveType::PlaneInfinite:
		ImGui::DragFloat3("Position", &primPosition.x, 0.025f);
		if(ImGui::DragFloat3("Normal", &primNormal.x, 0.01f))
		{
			primNormal = Normalize(primNormal);
		}

		if(ImGui::Button("Add Plane Infinite"))
		{
			PlaneInfinite* plane = new PlaneInfinite(primPosition, primNormal);
			sceneManager->AddPrimitiveToScene(plane);
			sceneUpdated = true;
		}
		break;
	}

	ImGui::End();
}

void Editor::ImGuiStyleSettings()
{
	ImGuiIO& io = ImGui::GetIO();

	// Fonts //	
	io.FontDefault = io.Fonts->AddFontFromFileTTF("Assets/Fonts/Roboto-Regular.ttf", 13.f);
	io.Fonts->AddFontFromFileTTF("Assets/Fonts/Roboto-Bold.ttf", 13.f);

	baseFont = ImGui::GetFont();
	boldFont = io.Fonts->Fonts[1];


	// Style //
	ImGuiStyle& style = ImGui::GetStyle();
	style.ScrollbarRounding = 2;
	style.ScrollbarSize = 12;
	style.WindowRounding = 3;
	style.WindowBorderSize = 0.0f;
	style.WindowTitleAlign = ImVec2(0.0, 0.5f);
	style.WindowPadding = ImVec2(5, 1);
	style.ItemSpacing = ImVec2(12, 5);
	style.FrameBorderSize = 0.5f;
	style.FrameRounding = 3;
	style.GrabMinSize = 5;

	// Color Wheel //
	ImGui::SetColorEditOptions(ImGuiColorEditFlags_Float | ImGuiColorEditFlags_HDR | 
		ImGuiColorEditFlags_PickerHueBar);

	ImVec4* colors = ImGui::GetStyle().Colors;
	colors[ImGuiCol_Text] = ImVec4(0.761, 0.761, 0.761, 1.00f);
	colors[ImGuiCol_TextDisabled] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
	colors[ImGuiCol_WindowBg] = ImVec4(0.118f, 0.118f, 0.118f, 1.00f);
	colors[ImGuiCol_ChildBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
	colors[ImGuiCol_PopupBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
	colors[ImGuiCol_Border] = ImVec4(0.21f, 0.21f, 0.21f, 1.00f);
	colors[ImGuiCol_BorderShadow] = ImVec4(0.21f, 0.21f, 0.21f, 0.00f);
	colors[ImGuiCol_FrameBg] = ImVec4(0.06f, 0.06f, 0.06f, 1.00f);
	colors[ImGuiCol_FrameBgHovered] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
	colors[ImGuiCol_FrameBgActive] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
	colors[ImGuiCol_TitleBg] = ImVec4(0.16f, 0.16f, 0.16f, 1.00f);
	colors[ImGuiCol_TitleBgActive] = ImVec4(0.16f, 0.16f, 0.16f, 1.00f);
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.16f, 0.16f, 0.16f, 1.00f);
	colors[ImGuiCol_MenuBarBg] = ImVec4(0.16f, 0.16f, 0.16f, 1.00f);
	colors[ImGuiCol_ScrollbarBg] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
	colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.34f, 0.34f, 0.34f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.43f, 0.43f, 0.43f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.43f, 0.43f, 0.43f, 1.00f);
	colors[ImGuiCol_CheckMark] = ImVec4(0.85f, 0.48f, 0.21f, 1.00f);
	colors[ImGuiCol_SliderGrab] = ImVec4(0.85f, 0.48f, 0.21f, 1.00f);
	colors[ImGuiCol_SliderGrabActive] = ImVec4(0.96f, 0.72f, 0.55f, 1.00f);
	colors[ImGuiCol_Button] = ImVec4(0.22f, 0.22f, 0.22f, 1.00f);
	colors[ImGuiCol_ButtonHovered] = ImVec4(0.29f, 0.29f, 0.29f, 1.00f);
	colors[ImGuiCol_ButtonActive] = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
	colors[ImGuiCol_Header] = ImVec4(0.18f, 0.18f, 0.18f, 1.00f);
	colors[ImGuiCol_HeaderHovered] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
	colors[ImGuiCol_HeaderActive] = ImVec4(0.27f, 0.27f, 0.27f, 1.00f);
	colors[ImGuiCol_Separator] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
	colors[ImGuiCol_SeparatorHovered] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
	colors[ImGuiCol_SeparatorActive] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
	colors[ImGuiCol_ResizeGrip] = ImVec4(0.18f, 0.18f, 0.18f, 1.00f);
	colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.22f, 0.22f, 0.22f, 1.00f);
	colors[ImGuiCol_ResizeGripActive] = ImVec4(0.35f, 0.35f, 0.35f, 1.00f);
	colors[ImGuiCol_Tab] = ImVec4(0.18f, 0.18f, 0.18f, 1.00f);
	colors[ImGuiCol_TabHovered] = ImVec4(0.22f, 0.22f, 0.22f, 1.00f);
	colors[ImGuiCol_TabActive] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
	colors[ImGuiCol_TabUnfocused] = ImVec4(0.18f, 0.18f, 0.18f, 1.00f);
	colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.18f, 0.18f, 0.18f, 1.00f);
	colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
	colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
	colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
	colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
	colors[ImGuiCol_TableHeaderBg] = ImVec4(0.19f, 0.19f, 0.20f, 1.00f);
	colors[ImGuiCol_TableBorderStrong] = ImVec4(0.31f, 0.31f, 0.35f, 1.00f);
	colors[ImGuiCol_TableBorderLight] = ImVec4(0.23f, 0.23f, 0.25f, 1.00f);
	colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
	colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
	colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
	colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
	colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
	colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
}