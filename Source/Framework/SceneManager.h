#pragma once
#include "Graphics/Primitive.h"

#include <vector>
#include <string>

struct Scene
{
	std::string Name;
	std::vector<Primitive*> primitives;
};

class SceneManager
{
public:
	SceneManager();
	~SceneManager();

	// Scene Serialization //
	void LoadScene(const std::string& sceneName);
	void SaveScene();

	// Scene Management //
	void AddPrimitiveToScene(Primitive* primitive);
	void UpdateScene();
	
	Scene* GetActiveScene();

private:
	Scene* activeScene;
	std::string lastSceneSettings = "Scenes/scene.settings";

	std::vector<Primitive*> primitiveBackBuffer;
};