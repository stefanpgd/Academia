#include "SceneManager.h"
#include <fstream>

// Primitives //
#include "Graphics/Sphere.h"
#include "Graphics/Plane.h"
#include "Graphics/PlaneInfinite.h"
#include "Graphics/Triangle.h"

#include "Utilities/LogHelper.h"

SceneManager::SceneManager()
{
	LOG("Checking for last used scene...");

	std::ifstream lastScene(lastSceneSettings);

	// Grab info about the last open scene
	if(lastScene.is_open())
	{
		LOG("Last used scene found!");
		
		std::string path;
		std::getline(lastScene, path);
		LoadScene(path);
	}
	else
	{
		LOG(Log::MessageType::Debug, "No last used scene found, loading default scene...");

		// TO-DO: catch the issue with no objects in the scene to intersect
		activeScene = new Scene();
		activeScene->Name = "Default";

		Sphere* sphere = new Sphere(vec3(0.0f), 0.25f);
		activeScene->primitives.push_back(sphere);
	}

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

void SceneManager::LoadScene(const std::string& sceneName)
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
	
	// Scene Info //
	std::getline(scene, line);
	activeScene->Name = line;

	std::getline(scene, line);
	int amountOfPrimitives = std::stoi(line);

	for(int i = 0; i < amountOfPrimitives; i++)
	{
		Primitive* primitive;

		// Primitive Type info //
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

		default:
			primitive = new Sphere(vec3(0.0f), 0.1f);
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
		primitive->Material.EmissiveStrength = std::stof(line);

		std::getline(scene, line);
		primitive->Material.isEmissive = std::stoi(line);

		std::getline(scene, line);
		primitive->Material.isDielectric = std::stoi(line);

		activeScene->primitives.push_back(primitive);
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
	sceneFile << activeScene->primitives.size() << "\n";

	for(int i = 0; i < activeScene->primitives.size(); i++)
	{
		sceneFile << int(activeScene->primitives[i]->Type) << "\n";

		// Primitive specific data // 
		switch(activeScene->primitives[i]->Type)
		{
		case PrimitiveType::Sphere:
			for(int j = 0; j < 3; j++)
			{
				sceneFile << activeScene->primitives[i]->Position.data[j] << "\n";
			}

			Sphere* sphere = dynamic_cast<Sphere*>(activeScene->primitives[i]);
			sceneFile << sphere->Radius << "\n";
			break;
		}

		// Material Properties // 
		for(int j = 0; j < 3; j++)
		{
			sceneFile << activeScene->primitives[i]->Material.Color.data[j] << "\n";
		}

		sceneFile << activeScene->primitives[i]->Material.Specularity << "\n";
		sceneFile << activeScene->primitives[i]->Material.Roughness << "\n";
		sceneFile << activeScene->primitives[i]->Material.Metalness << "\n";
		sceneFile << activeScene->primitives[i]->Material.IoR << "\n";
		sceneFile << activeScene->primitives[i]->Material.EmissiveStrength << "\n";
		sceneFile << activeScene->primitives[i]->Material.isEmissive << "\n";
		sceneFile << activeScene->primitives[i]->Material.isDielectric << "\n";
	}

	LOG("Scene succesfully saved!");
}

Scene* SceneManager::GetActiveScene()
{
	return activeScene;
}
