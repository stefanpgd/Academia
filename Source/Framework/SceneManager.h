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

	void LoadScene(const std::string& sceneName);
	void SaveScene();

	Scene* GetActiveScene();

private:
	Scene* activeScene;
	std::string lastSceneSettings = "Scenes/scene.settings";
};