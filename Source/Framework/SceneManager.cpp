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

	if(scene.is_open())
	{
		activeScene = new Scene();
		std::getline(scene, line);

		activeScene->Name = line;

		vec3 position;
		for(int i = 0; i < 3; i++)
		{
			std::getline(scene, line);
			position.data[i] = std::stof(line);
		}
		
		Sphere* sphere = new Sphere(position, 0.25f);

		vec3 color;
		for(int i = 0; i < 3; i++)
		{
			std::getline(scene, line);
			color.data[i] = std::stof(line);
		}
		sphere->material.Color = color;

		std::getline(scene, line);
		sphere->material.Specularity = std::stof(line);

		std::getline(scene, line);
		sphere->material.Roughness = std::stof(line);

		std::getline(scene, line);
		sphere->material.Metalness = std::stof(line);

		std::getline(scene, line);
		sphere->material.IoR = std::stof(line);

		std::getline(scene, line);
		sphere->material.EmissiveStrength = std::stof(line);

		std::getline(scene, line);
		sphere->material.isEmissive = std::stof(line);

		std::getline(scene, line);
		sphere->material.isDielectric = std::stof(line);

		activeScene->primitives.push_back(sphere);
	}
	else
	{
		LOG(Log::MessageType::Error, "Tried loading a scene that doesn't exist!");
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

	// For now, writing the data of just a sphere
	for(int i = 0; i < 3; i++)
	{
		sceneFile << activeScene->primitives[0]->Position.data[i] << "\n";
	}

	for(int i = 0; i < 3; i++)
	{
		sceneFile << activeScene->primitives[0]->material.Color.data[i] << "\n";
	}

	sceneFile << activeScene->primitives[0]->material.Specularity << "\n";
	sceneFile << activeScene->primitives[0]->material.Roughness << "\n";
	sceneFile << activeScene->primitives[0]->material.Metalness << "\n";
	sceneFile << activeScene->primitives[0]->material.IoR << "\n";
	sceneFile << activeScene->primitives[0]->material.EmissiveStrength << "\n";
	sceneFile << activeScene->primitives[0]->material.isEmissive << "\n";
	sceneFile << activeScene->primitives[0]->material.isDielectric << "\n";

	LOG("Scene succesfully saved!");
}

Scene* SceneManager::GetActiveScene()
{
	return activeScene;
}
