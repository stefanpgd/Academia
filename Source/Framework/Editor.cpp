#include "Editor.h"
#include <filesystem>

#include "Framework/App.h"
#include "Framework/Input.h"
#include "Framework/Renderer.h"
#include "Framework/SceneManager.h"

#include "Graphics/RayTracer.h"
#include "Graphics/Sphere.h"
#include "Graphics/PlaneInfinite.h"

#include "Utilities/LogHelper.h"

Editor::Editor(GLFWwindow* window, App* app) : app(app)
{
	sceneManager = app->renderer->sceneManager;
	activeScene = sceneManager->GetActiveScene();

	LoadEXRFilePaths();

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

void Editor::Update()
{
	sceneUpdated = false;

	if(Input::GetKey(KeyCode::H))
	{
		// implement GetKeyDown...
		renderEditor = !renderEditor;
	}

	if(!renderEditor)
	{
		return;
	}

	//if(selectedPrimitive != app->nearestPrimitive)
	//{
	//	selectedPrimitive = app->nearestPrimitive;
	//}

	MenuBar();

	PrimitiveSelection();
	PrimitiveHierarchy();
	PrimitiveCreation();

	SceneSettings();

	if(sceneUpdated)
	{
		activeScene->HasUpdated = true;
	}
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

void Editor::MenuBar()
{
	//const int staticsWidth = 800; 
	//int FPSOffset = (app->screenWidth - staticsWidth);
	//
	//if(ImGui::BeginMainMenuBar())
	//{
	//	//ImGui::Checkbox("Lock Movement", &app->lockUserMovement);
	//
	//	// Window Selection //
	//	if(ImGui::BeginMenu("Windows"))
	//	{
	//		if (ImGui::MenuItem("Primitive Selection", NULL, &showPrimitiveSelection)) {}
	//		if (ImGui::MenuItem("Primitive Creation", NULL, &showPrimitiveCreation)) {}
	//		if (ImGui::MenuItem("Scene Hierarchy", NULL, &showSceneHierarchy)) {}
	//		if (ImGui::MenuItem("Scene Settings", NULL, &showSceneSettings)) {}
	//
	//		ImGui::EndMenu();
	//	}
	//
	//	if(ImGui::Button("Take Screenshot"))
	//	{
	//		//app->takeScreenshot = true;
	//	}
	//
	//	ImGui::Dummy(ImVec2(FPSOffset, 0));
	//
	//	// FPS // 
	//	ImGui::PushFont(boldFont);
	//	ImGui::Text("FPS:");
	//	ImGui::PopFont();
	//
	//	std::string fps = std::to_string(int(1.0f / app->deltaTime));
	//	ImGui::Text(fps.c_str());
	//
	//	ImGui::Separator();
	//
	//	// Average FPS //
	//	float average = 0.0f;
	//	for(int i = 0; i < app->FPSLogSize; i++)
	//	{
	//		average += app->FPSLog[i];
	//	}
	//	average /= app->FPSLogSize;
	//
	//	ImGui::PushFont(boldFont);
	//	ImGui::Text("Average FPS:");
	//	ImGui::PopFont();
	//
	//	std::string averageFPS = std::to_string(int(1.0f / average));
	//	ImGui::Text(averageFPS.c_str());
	//
	//	ImGui::Separator();
	//
	//	// Samples Taken //
	//	ImGui::PushFont(boldFont);
	//	ImGui::Text("Samples Taken:");
	//	ImGui::PopFont();
	//
	//	std::string samplesTaken = std::to_string(app->frameCount);
	//	ImGui::Text(samplesTaken.c_str());
	//
	//	ImGui::Separator();
	//
	//	// Time Elasped // 
	//	ImGui::PushFont(boldFont);
	//	ImGui::Text("Time Elapsed:");
	//	ImGui::PopFont();
	//
	//	int min = int(app->timeElasped / 60.0f);
	//	std::string minutes = std::to_string(min);
	//	if(min < 10)
	//	{
	//		minutes = "0" + minutes;
	//	}
	//
	//	int sec = int(app->timeElasped) % 60;
	//	std::string seconds = std::to_string(sec);
	//	if(sec < 10)
	//	{
	//		seconds = "0" + seconds;
	//	}
	//
	//	float milli = app->timeElasped - int(app->timeElasped);
	//	std::string milliseconds = std::to_string(int(milli * 100.0f));
	//	if(milli < 0.1f)
	//	{
	//		milliseconds = "0" + milliseconds;
	//	}
	//
	//	std::string time = minutes + ":" + seconds + ":" + milliseconds;
	//	ImGui::Text(time.c_str());
	//
	//	ImGui::Separator();
	//
	//	ImGui::EndMainMenuBar();
	//}
}

void Editor::SceneSettings()
{
	if (!showSceneSettings)
	{
		return;
	}

	// Window Positioning & Flags //
	const int width = 350;
	const int height = 200;
	const int x = app->screenWidth - width;
	int y = 18;

	// If selectedPrimitive is not 'NUll/nullptr' 
	// then the PrimitiveSelection window is open, thus it needs to move down
	if (selectedPrimitive)
	{
		y += 365;
	}

	ImGui::SetNextWindowPos(ImVec2(x, y));
	ImGui::SetNextWindowSize(ImVec2(width, height));

	ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse;

	ImGui::PushFont(boldFont);
	ImGui::Begin("Scene Settings", nullptr, flags);
	ImGui::PushFont(baseFont);

	if(ImGui::CollapsingHeader("Path Tracer Settings"))
	{
		PathTracerSettings();
	}

	if(ImGui::CollapsingHeader("Skydome Settings"))
	{
		SkydomeSettings();
	}

	if(ImGui::CollapsingHeader("Camera Settings"))
	{
		CameraSettings();
	}

	ImGui::PopFont();
	ImGui::End();
	ImGui::PopFont();
}

void Editor::PathTracerSettings()
{
	//ImGui::Columns(2);

	//ImGui::Separator();
	//ImGui::AlignTextToFramePadding();
	//ImGui::Text("Target Sample Count");
	//ImGui::NextColumn();
	//if(ImGui::DragInt("##0", &app->targetSampleCount, 50, 0, 1000000)) 
	//{ 
	//	sceneUpdated = true; 
	//	app->updateScreenBuffer = true; 
	//}
	//ImGui::NextColumn();

	//ImGui::Separator();
	//ImGui::AlignTextToFramePadding();
	//ImGui::Text("Max Luminance Per Frame");
	//ImGui::NextColumn();
	//if(ImGui::DragFloat("##1", &app->rayTracer->maxLuminance, 0.1f, 0.0f, 1000.0f)) { sceneUpdated = true; }
	//ImGui::NextColumn();

	//ImGui::Separator();
	//ImGui::AlignTextToFramePadding();
	//ImGui::Text("Ray Depth");
	//ImGui::NextColumn();
	//if(ImGui::DragInt("##2", &app->rayTracer->maxRayDepth, 0.02f, 1, 100)) { sceneUpdated = true; }
	//ImGui::NextColumn();

	//ImGui::Separator();
	//ImGui::AlignTextToFramePadding();
	//ImGui::Text("Use Skydome");
	//ImGui::NextColumn();
	//if(ImGui::Checkbox("##4", &app->rayTracer->useSkydomeTexture)) { sceneUpdated = true; }

	//ImGui::Columns(1);
	//ImGui::Separator();
}

void Editor::SkydomeSettings()
{
	//ImGui::PushFont(boldFont);
	//ImGui::SeparatorText("Skydome");
	//ImGui::PopFont();

	//ImGui::Columns(2);

	//ImGui::Separator();
	//ImGui::AlignTextToFramePadding();
	//ImGui::Text("Active Skydome");
	//ImGui::NextColumn();
	//
	//int currentItem = 0;
	//for(int i = 0; i < exrFilePaths.size(); i++)
	//{
	//	if(app->rayTracer->skydomePath == exrFilePaths[i])
	//	{
	//		currentItem = i;
	//	}
	//}

	//if(ImGui::BeginCombo("##5", app->rayTracer->skydomePath.c_str()))
	//{
	//	for(int i = 0; i < exrFilePaths.size(); i++)
	//	{
	//		bool isSelected = currentItem == i;

	//		if(ImGui::Selectable(exrFilePaths[i].c_str(), isSelected))
	//		{
	//			app->rayTracer->skydomePath = exrFilePaths[i];
	//			app->reloadSkydome = true;
	//			sceneUpdated = true;
	//		}

	//		if(isSelected)
	//		{
	//			ImGui::SetItemDefaultFocus();
	//		}
	//	}
	//	ImGui::EndCombo();
	//}

	//ImGui::NextColumn();


	//ImGui::Separator();
	//ImGui::AlignTextToFramePadding();
	//ImGui::Text("Emission");
	//ImGui::NextColumn();
	//if(ImGui::DragFloat("##6", &app->rayTracer->scene->SkyDomeEmission, 0.01f)) { sceneUpdated = true; }
	//ImGui::NextColumn();

	//ImGui::Separator();
	//ImGui::AlignTextToFramePadding();
	//ImGui::Text("Background Strength");
	//ImGui::NextColumn();
	//if(ImGui::DragFloat("##7", &app->rayTracer->scene->SkyDomeBackgroundStrength, 0.01f, 0.0f, 10.0f)) { sceneUpdated = true; }
	//ImGui::NextColumn();

	//ImGui::Separator();
	//ImGui::AlignTextToFramePadding();
	//ImGui::Text("Orientation");
	//ImGui::NextColumn();
	//if(ImGui::SliderFloat("##8", &app->rayTracer->scene->SkydomeOrientation, 0.0f, 1.0f)) { sceneUpdated = true; }
	//ImGui::NextColumn();

	//ImGui::Columns(1);
	//ImGui::Separator();

	//ImGui::PushFont(boldFont);
	//ImGui::SeparatorText("Sky Color");
	//ImGui::PopFont();

	//ImGui::Columns(2);

	//ImGui::Separator();
	//ImGui::AlignTextToFramePadding();
	//ImGui::Text("Color - Up");
	//ImGui::NextColumn();
	//if(ImGui::ColorEdit3("##9", &app->rayTracer->skyColorB.x)) { sceneUpdated = true; }
	//ImGui::NextColumn();

	//ImGui::Separator();
	//ImGui::AlignTextToFramePadding();
	//ImGui::Text("Color - Down");
	//ImGui::NextColumn();
	//if(ImGui::ColorEdit3("##10", &app->rayTracer->skyColorA.x)) { sceneUpdated = true; }
	//ImGui::NextColumn();

	//ImGui::Columns(1);
	//ImGui::Separator();
}

void Editor::CameraSettings()
{
	/*Camera* camera = app->rayTracer->camera;
	bool cameraUpdated = false;

	ImGui::Columns(2);

	ImGui::Separator();
	ImGui::AlignTextToFramePadding();
	ImGui::Text("Position");
	ImGui::NextColumn();
	if(ImGui::DragFloat3("##11", &camera->Position.x, 0.05f, 0.0f, 0.0f, "%.2f")) { cameraUpdated = true; }
	ImGui::NextColumn();

	ImGui::Separator();
	ImGui::AlignTextToFramePadding();
	ImGui::Text("View Direction");
	ImGui::NextColumn();
	if(ImGui::DragFloat3("##12", &camera->ViewDirection.x, 0.01f, 0.0f, 0.0f, "%.2f")) { cameraUpdated = true; }
	ImGui::NextColumn();

	ImGui::Separator();
	ImGui::AlignTextToFramePadding();
	ImGui::Text("Move Speed");
	ImGui::NextColumn();
	if(ImGui::DragFloat("##13", &camera->Speed, 0.01f, 0.0f, 100.0f)) { cameraUpdated = true; }
	ImGui::NextColumn();

	ImGui::Separator();
	ImGui::AlignTextToFramePadding();
	ImGui::Text("Boost Multiplier");
	ImGui::NextColumn();
	if(ImGui::DragFloat("##14", &camera->BoostMultiplier, 0.01f, 0.0f, 100.0f)) { cameraUpdated = true; }
	ImGui::NextColumn();

	ImGui::Separator();
	ImGui::AlignTextToFramePadding();
	ImGui::Text("Slow Multiplier");
	ImGui::NextColumn();
	if(ImGui::DragFloat("##15", &camera->SlowMultiplier, 0.01f, 0.0f, 100.0f)) { cameraUpdated = true; }
	ImGui::NextColumn();

	ImGui::Columns(1);

	if(cameraUpdated)
	{
		camera->SetupVirtualPlane(app->screenWidth, app->screenHeight);
		sceneUpdated = true;
	}*/
}

void Editor::PrimitiveSelection()
{
	if (!showPrimitiveSelection)
	{
		return;
	}

	// if no primitive is selected, than there is no primitive
	// to showcase details for.
	if(!selectedPrimitive)
	{
		return;
	}

	// Window Positioning & Flags //
	const int width = 350;
	const int height = 365;
	const int x = app->screenWidth - width;
	const int y = 19;

	ImGui::SetNextWindowPos(ImVec2(x, y));
	ImGui::SetNextWindowSize(ImVec2(width, height));

	ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse;
	Primitive* primitive = selectedPrimitive;
	Material* material = &primitive->Material;

	placeholderName = primitive->name;

	ImGui::PushFont(boldFont);
	ImGui::Begin("Primitive Editor", nullptr, flags);
	ImGui::PushFont(baseFont);

	std::string info = "Primitive Properties - " + primitive->name;

	ImGui::PushFont(boldFont);
	ImGui::SeparatorText(info.c_str());
	ImGui::PopFont();
	ImGui::PushFont(baseFont);
	ImGui::PopFont();

	ImGui::Columns(2);

	ImGui::Separator();
	ImGui::AlignTextToFramePadding();
	ImGui::Text("Name");
	ImGui::NextColumn();
	if(ImGui::InputText("##1", &placeholderName)) { primitive->name = placeholderName; }
	ImGui::NextColumn();

	ImGui::Separator();
	ImGui::AlignTextToFramePadding();
	ImGui::Text("Position");
	ImGui::NextColumn();
	if(ImGui::DragFloat3("##2", &primitive->Position.x, 0.05f, 0.0f, 0.0f, "%.2f")) { sceneUpdated = true; }
	ImGui::NextColumn();

	switch(primitive->Type)
	{
	case PrimitiveType::Sphere:
		Sphere* sphere = dynamic_cast<Sphere*>(primitive);

		ImGui::Separator();
		ImGui::AlignTextToFramePadding();
		ImGui::Text("Scale");
		ImGui::NextColumn();
		if(ImGui::InputFloat("##3", &sphere->Radius, 0.1f, 0.5f))
		{
			sphere->Radius2 = sphere->Radius * sphere->Radius;
			sceneUpdated = true;
		}
		ImGui::NextColumn();
		break;
	}
	ImGui::Columns(1);
	ImGui::Separator();

	ImGui::PushFont(boldFont);
	ImGui::SeparatorText("Material Properties");
	ImGui::PopFont();
	ImGui::PushFont(baseFont);
	ImGui::PopFont();

	ImGui::Columns(2);

	ImGui::Separator();
	ImGui::AlignTextToFramePadding();
	ImGui::Text("Color");
	ImGui::NextColumn();
	if(ImGui::ColorEdit3("##4", &material->Color.x, 0.01f)) { sceneUpdated = true; }
	ImGui::NextColumn();

	if(material->isDielectric)
	{
		ImGui::Separator();
		ImGui::AlignTextToFramePadding();
		ImGui::Text("Index of Refraction");
		ImGui::NextColumn();
		if(ImGui::DragFloat("##5", &material->IoR, 0.002f, 1.0f, 3.0f)) { sceneUpdated = true; }
		ImGui::NextColumn();

		ImGui::Separator();
		ImGui::AlignTextToFramePadding();
		ImGui::Text("Density");
		ImGui::NextColumn();
		if(ImGui::DragFloat("##6", &material->Density, 0.01, 0.0f, 100.0f)) { sceneUpdated = true; }
		ImGui::NextColumn();

		ImGui::Separator();
		ImGui::AlignTextToFramePadding();
		ImGui::Text("Is Dielectric");
		ImGui::NextColumn();
		if(ImGui::Checkbox("##7", &material->isDielectric)) { sceneUpdated = true; }
		ImGui::NextColumn();
	}
	else if(material->isEmissive)
	{
		ImGui::Separator();
		ImGui::AlignTextToFramePadding();
		ImGui::Text("Emissive Strength");
		ImGui::NextColumn();
		if(ImGui::DragFloat("##5", &material->EmissiveStrength, 0.02f, 0.0f, 100.0f)) { sceneUpdated = true; }
		ImGui::NextColumn();

		ImGui::Separator();
		ImGui::AlignTextToFramePadding();
		ImGui::Text("Is Emissive");
		ImGui::NextColumn();
		if(ImGui::Checkbox("##6", &material->isEmissive)) { sceneUpdated = true; }
		ImGui::NextColumn();
	}
	else // then material is Opaque model
	{
		ImGui::Separator();
		ImGui::AlignTextToFramePadding();
		ImGui::Text("Specularity");
		ImGui::NextColumn();
		if(ImGui::DragFloat("##5", &material->Specularity, 0.002f, 0.0f, 1.0f)) { sceneUpdated = true; }
		ImGui::NextColumn();

		ImGui::Separator();
		ImGui::AlignTextToFramePadding();
		ImGui::Text("Roughness");
		ImGui::NextColumn();
		if(ImGui::DragFloat("##6", &material->Roughness, 0.002f, 0.0f, 1.0f)) { sceneUpdated = true; }
		ImGui::NextColumn();

		ImGui::Separator();
		ImGui::AlignTextToFramePadding();
		ImGui::Text("Metalness");
		ImGui::NextColumn();
		if(ImGui::DragFloat("##7", &material->Metalness, 0.002f, 0.0f, 1.0f)) { sceneUpdated = true; }
		ImGui::NextColumn();

		ImGui::Separator();
		ImGui::AlignTextToFramePadding();
		ImGui::Text("Index of Refraction");
		ImGui::NextColumn();
		if(ImGui::DragFloat("##8", &material->IoR, 0.002f, 1.0f, 3.0f)) { sceneUpdated = true; }
		ImGui::NextColumn();

		ImGui::Separator();
		ImGui::AlignTextToFramePadding();
		ImGui::Text("Is Dielectric");
		ImGui::NextColumn();
		if(ImGui::Checkbox("##9", &material->isDielectric)) { sceneUpdated = true; }
		ImGui::NextColumn();

		ImGui::Separator();
		ImGui::AlignTextToFramePadding();
		ImGui::Text("Is Emissive");
		ImGui::NextColumn();
		if(ImGui::Checkbox("##10", &material->isEmissive)) { sceneUpdated = true; }
		ImGui::NextColumn();
	}

	ImGui::Columns(1);
	ImGui::Separator();

	ImGui::PopFont();
	ImGui::End();
	ImGui::PopFont();
}

void Editor::PrimitiveHierarchy()
{
	//if (!showSceneHierarchy)
	//{
	//	return;
	//}

	//// Window Positioning & Flags //
	//const int width = 300;
	//const int height = 300;
	//const int x = 0;
	//const int y = 19;

	//ImGui::SetNextWindowPos(ImVec2(x, y));
	//ImGui::SetNextWindowSize(ImVec2(width, height));

	//ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse;
	//std::vector<Primitive*>& primitives = app->rayTracer->scene->primitives;

	//ImGui::PushFont(boldFont);
	//ImGui::Begin("Scene Hierarchy", NULL, flags);
	//ImGui::PushFont(baseFont);

	//ImGui::Separator();
	//ImGui::Indent(8.0f);

	//for(int i = 0; i < primitives.size(); i++)
	//{
	//	ImGui::PushID(i);

	//	ImGui::Bullet();

	//	Primitive* primitive = primitives[i];
	//	std::string name = primitive->name.c_str();

	//	bool isSelected = primitive == selectedPrimitive;
	//	if(ImGui::Selectable(name.c_str(), isSelected))
	//	{
	//		selectedPrimitive = primitive;
	//		app->nearestPrimitive = primitive;
	//	}

	//	if(isSelected)
	//	{
	//		ImGui::SetItemDefaultFocus();
	//	}

	//	ImGui::SameLine();
	//	ImGui::Text("-");
	//	ImGui::SameLine();
	//	ImGui::Text(primitiveNames[int(primitive->Type)]);
	//	
	//	ImGui::SameLine();
	//	ImGui::Text("-");

	//	ImGui::SameLine();
	//	if(primitive->Material.isDielectric)
	//	{
	//		ImGui::Text("Dielectric");
	//	}
	//	else if(primitive->Material.isEmissive)
	//	{
	//		ImGui::Text("Emissive");
	//	}
	//	else if(primitive->Material.Specularity > 0.99f)
	//	{
	//		ImGui::Text("Pure Specular");
	//	}
	//	else
	//	{
	//		ImGui::Text("Opaque");
	//	}

	//	ImGui::PopID();
	//}

	//ImGui::Unindent(8.0f);
	//ImGui::Separator();

	//ImGui::PopFont();
	//ImGui::End();
	//ImGui::PopFont();
}

void Editor::PrimitiveCreation()
{
	if (!showPrimitiveCreation)
	{
		return;
	}

	ImGui::Begin("Primitive Creation");

	if(ImGui::BeginCombo("Primitive Type", primitiveNames[selectedPrimitiveType]))
	{
		for(int i = 0; i < primitiveNames.size(); i++)
		{
			bool isSelected = primitiveNames[i] == primitiveNames[selectedPrimitiveType];

			if(ImGui::Selectable(primitiveNames[i], isSelected))
			{
				selectedPrimitiveType = i;
			}

			if(isSelected)
			{
				ImGui::SetItemDefaultFocus();
			}
		}

		ImGui::EndCombo();
	}

	PrimitiveType type = PrimitiveType(selectedPrimitiveType);
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

void Editor::LoadEXRFilePaths()
{
	std::string path = "Assets/EXRs/";
	for(const auto& file : std::filesystem::directory_iterator(path))
	{
		if(file.is_directory())
		{
			continue;
		}

		std::string filePath = file.path().string();
		std::string fileType = filePath.substr(filePath.find_last_of('.') + 1, filePath.size());

		if(fileType == "exr")
		{
			exrFilePaths.push_back(filePath);
		}
	}

	std::string exrCount = std::to_string(exrFilePaths.size());
	LOG("Found '" + exrCount + "' usable EXRs inside of 'Assets/EXRs'");
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