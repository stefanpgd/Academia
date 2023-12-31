#pragma once
#include "Graphics/Primitive.h"

#include <vector>
#include <string>

class Camera;

struct Skydome
{
	std::string Name = "Studio";
	int width, height, comp;
	float* image;

	// Skydome //
	float SkydomeOrientation = 0.0f;
	float SkyDomeEmission = 1.0f;
	float SkyDomeBackgroundStrength = 1.0f;
};

struct Scene
{
	std::string Name;
	std::vector<Primitive*> primitives;

	Camera* Camera;
	Skydome Skydome;

	// Whenever the camera, skydome or any primitive in the scene
	// gets updated, this flipped to notify that we need restart sampling
	bool HasUpdated = false;
};

class SceneManager
{
public:
	SceneManager(unsigned int screenWidth, unsigned int screenHeight);
	~SceneManager();

	bool Update(float deltaTime);

	// Scene Serialization //
	void LoadScene(const std::string& sceneName, unsigned int screenWidth, unsigned int screenHeight);
	void LoadSkydome(const std::string& skydomePath);
	void SaveScene();

	// Scene Management //
	void AddPrimitiveToScene(Primitive* primitive);
	void UpdateScene();

	Scene* GetActiveScene();

private:

private:
	Scene* activeScene;
	std::string lastSceneSettings = "Scenes/scene.settings";
	std::vector<Primitive*> primitiveBackBuffer;

	bool lockCameraMovement = false;

	// Skydome //
	bool reloadSkydome = false;
	std::string skydomeToLoad;

	friend class Editor;
};