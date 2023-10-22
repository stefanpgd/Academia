#pragma once
#include "Graphics/Primitive.h"

#include <vector>
#include <string>

class Camera;

struct Scene
{
	std::string Name;
	std::vector<Primitive*> primitives;

	Camera* Camera;
};

class SceneManager
{
public:
	SceneManager(unsigned int screenWidth, unsigned int screenHeight);
	~SceneManager();

	// Scene Serialization //
	void LoadScene(const std::string& sceneName, unsigned int screenWidth, unsigned int screenHeight);
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