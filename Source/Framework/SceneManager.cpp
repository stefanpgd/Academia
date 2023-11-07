#include "SceneManager.h"
#include <fstream>
#include <stb_image.h>
#include <tinyexr.h>

#include "Graphics/Camera.h"

// Primitives //
#include "Graphics/Sphere.h"
#include "Graphics/Plane.h"
#include "Graphics/PlaneInfinite.h"
#include "Graphics/Triangle.h"

#include "Utilities/LogHelper.h"

SceneManager::SceneManager(unsigned int screenWidth, unsigned int screenHeight)
{
	LOG("Checking for last used scene...");
	std::ifstream lastScene(lastSceneSettings);

	if(lastScene.is_open())
	{
		LOG("Last used scene found!");

		std::string path;
		std::getline(lastScene, path);
		LoadScene(path, screenWidth, screenHeight);
	}
	else
	{
		LOG(Log::MessageType::Debug, "No last used scene found, loading default scene...");

		activeScene = new Scene();
		activeScene->Name = "Default";
		activeScene->Camera = new Camera(screenWidth, screenHeight);
	}

	// Replace with last loaded skydome // 
	LoadSkydome("Assets/EXRs/studio.exr");

	LOG("Scene succesfully loaded!");
}

SceneManager::~SceneManager()
{
	SaveScene();

	std::ofstream lastScene;
	lastScene.open(lastSceneSettings, std::fstream::out);
	lastScene.clear();

	std::string activeScenePath = "Scenes/" + activeScene->Name + ".scene";
	lastScene << activeScenePath;
}

bool SceneManager::Update(float deltaTime)
{
	bool cameraUpdated = false;

	if(!lockCameraMovement)
	{
		cameraUpdated = activeScene->Camera->Update(deltaTime);
	}

	return cameraUpdated || activeScene->HasUpdated;
}

void SceneManager::LoadScene(const std::string& sceneName, unsigned int screenWidth, unsigned int screenHeight)
{
	LOG("Loading Scene: '" + sceneName + "'");

	std::string line;
	std::ifstream scene(sceneName);

	if(!scene.is_open())
	{
		LOG(Log::MessageType::Error, "Tried loading a scene that doesn't exist!");
		return;
	}

	activeScene = new Scene();

	// Scene Information //
	std::getline(scene, line);
	activeScene->Name = line;

	// Camera Information //
	vec3 cameraPosition;
	for(int i = 0; i < 3; i++)
	{
		std::getline(scene, line);
		cameraPosition.data[i] = std::stof(line);
	}
	activeScene->Camera = new Camera(cameraPosition, screenWidth, screenHeight);

	// Skydome Information //
	std::getline(scene, line);
	activeScene->Skydome.SkydomeOrientation = std::stof(line);

	std::getline(scene, line);
	activeScene->Skydome.SkyDomeEmission = std::stof(line);

	std::getline(scene, line);
	activeScene->Skydome.SkyDomeBackgroundStrength = std::stof(line);

	// Primitive Information //
	std::getline(scene, line);
	int amountOfPrimitives = std::stoi(line);

	for(int i = 0; i < amountOfPrimitives; i++)
	{
		Primitive* primitive;

		// Primitive Type specific Information //
		std::getline(scene, line);
		PrimitiveType type = PrimitiveType(std::stoi(line));

		switch(type)
		{
		case PrimitiveType::Sphere:
		{
			vec3 position;
			for(int i = 0; i < 3; i++)
			{
				std::getline(scene, line);
				position.data[i] = std::stof(line);
			}

			std::getline(scene, line);
			float radius = std::stof(line);

			Sphere* sphere = new Sphere(position, radius);
			primitive = sphere;
			break;
		}

		case PrimitiveType::PlaneInfinite:
		{
			vec3 position;
			for(int i = 0; i < 3; i++)
			{
				std::getline(scene, line);
				position.data[i] = std::stof(line);
			}

			vec3 normal;
			for(int i = 0; i < 3; i++)
			{
				std::getline(scene, line);
				normal.data[i] = std::stof(line);
			}

			PlaneInfinite* plane = new PlaneInfinite(position, normal);
			primitive = plane;
			break;
		}

		default:
			Sphere* sphere = new Sphere(vec3(0.0f), 0.25f);
			primitive = sphere;
		}

		// Material Properties //
		vec3 color;
		for(int i = 0; i < 3; i++)
		{
			std::getline(scene, line);
			color.data[i] = std::stof(line);
		}
		primitive->Material.Color = color;

		std::getline(scene, line);
		primitive->Material.Specularity = std::stof(line);

		std::getline(scene, line);
		primitive->Material.Roughness = std::stof(line);

		std::getline(scene, line);
		primitive->Material.Metalness = std::stof(line);

		std::getline(scene, line);
		primitive->Material.IoR = std::stof(line);

		std::getline(scene, line);
		primitive->Material.Density = std::stof(line);

		std::getline(scene, line);
		primitive->Material.EmissiveStrength = std::stof(line);

		std::getline(scene, line);
		primitive->Material.isEmissive = std::stoi(line);

		std::getline(scene, line);
		primitive->Material.isDielectric = std::stoi(line);

		activeScene->primitives.push_back(primitive);
	}
}

void SceneManager::LoadSkydome(const std::string& skydomePath)
{
	Skydome& sd = activeScene->Skydome;
	delete sd.image;

	sd.Name = skydomePath;
	const char* err = nullptr;
	int result = LoadEXR(&sd.image, &sd.width, &sd.height, skydomePath.c_str(), &err);
	sd.comp = sizeof(float);

	if(result != TINYEXR_SUCCESS)
	{
		fprintf(stderr, "ERR : %s\n", err);
		FreeEXRErrorMessage(err);
	}
}

void SceneManager::SaveScene()
{
	LOG("Saving Scene: '" + activeScene->Name + "'");

	std::ofstream sceneFile;
	std::string path = "Scenes/" + activeScene->Name + ".scene";
	sceneFile.open(path, std::fstream::out);
	sceneFile.clear();

	sceneFile << activeScene->Name << "\n";

	// Camera Information //
	for(int i = 0; i < 3; i++)
	{
		sceneFile << activeScene->Camera->Position.data[i] << "\n";
	}

	// Skydome Information // 
	sceneFile << activeScene->Skydome.SkydomeOrientation << "\n";
	sceneFile << activeScene->Skydome.SkyDomeEmission << "\n";
	sceneFile << activeScene->Skydome.SkyDomeBackgroundStrength << "\n";

	sceneFile << activeScene->primitives.size() << "\n";
	for(int i = 0; i < activeScene->primitives.size(); i++)
	{
		sceneFile << int(activeScene->primitives[i]->Type) << "\n";

		// Primitive specific data // 
		switch(activeScene->primitives[i]->Type)
		{
		case PrimitiveType::Sphere:
		{
			for(int j = 0; j < 3; j++)
			{
				sceneFile << activeScene->primitives[i]->Position.data[j] << "\n";
			}

			Sphere* sphere = dynamic_cast<Sphere*>(activeScene->primitives[i]);
			sceneFile << sphere->Radius << "\n";
			break;
		}

		case PrimitiveType::PlaneInfinite:
		{
			for(int j = 0; j < 3; j++)
			{
				sceneFile << activeScene->primitives[i]->Position.data[j] << "\n";
			}

			PlaneInfinite* plane = dynamic_cast<PlaneInfinite*>(activeScene->primitives[i]);
			for(int j = 0; j < 3; j++)
			{
				sceneFile << plane->Normal.data[j] << "\n";
			}
			break;
		}
		}

		// Material Properties // 
		for(int j = 0; j < 3; j++)
		{
			sceneFile << activeScene->primitives[i]->Material.Color.data[j] << "\n";
		}

		// Opaque properties //
		sceneFile << activeScene->primitives[i]->Material.Specularity << "\n";
		sceneFile << activeScene->primitives[i]->Material.Roughness << "\n";
		sceneFile << activeScene->primitives[i]->Material.Metalness << "\n";

		// Dielectric properties //
		sceneFile << activeScene->primitives[i]->Material.IoR << "\n";
		sceneFile << activeScene->primitives[i]->Material.Density << "\n";
		sceneFile << activeScene->primitives[i]->Material.EmissiveStrength << "\n";

		// Emissive properties //
		sceneFile << activeScene->primitives[i]->Material.isEmissive << "\n";
		sceneFile << activeScene->primitives[i]->Material.isDielectric << "\n";
	}

	LOG("Scene succesfully saved!");
}

void SceneManager::AddPrimitiveToScene(Primitive* primitive)
{
	primitiveBackBuffer.push_back(primitive);
}

/// <summary>
/// Adds new primitives that where in the back buffer into the scene.
/// Also removes all primitves that have been marked for delete.
/// </summary>
void SceneManager::UpdateScene()
{
	// Remove 'MarkedForDelete' primitives //
	activeScene->primitives.erase(
		std::remove_if(activeScene->primitives.begin(), activeScene->primitives.end(),
			[](Primitive* primitive) { return primitive->MarkedForDelete; }),
			activeScene->primitives.end());

	// Add back-buffered primitives //
	if(primitiveBackBuffer.size() > 0)
	{
		for(int i = 0; i < primitiveBackBuffer.size(); i++)
		{
			activeScene->primitives.push_back(primitiveBackBuffer[i]);
		}

		primitiveBackBuffer.clear();
	}

	if(reloadSkydome)
	{
		LoadSkydome(skydomeToLoad);
		reloadSkydome = false;
	}

	activeScene->HasUpdated = false;
}

Scene* SceneManager::GetActiveScene()
{
	return activeScene;
}
