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

	// Skydome //
	float SkydomeOrientation = 0.0f;
	float SkyDomeEmission = 1.0f;
	float SkyDomeBackgroundStrength = 1.0f;

	// Whenever the camera, skydome or any primitive in the scene
	// gets updated, this flipped to notify that we need restart sampling
	bool HasUpdated = false;
};

class SceneManager
{
public:
	SceneManager(unsigned int screenWidth, unsigned int screenHeight);
	~SceneManager();

	bool Update();

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