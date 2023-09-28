#pragma once
#include "Graphics/Primitive.h"
#include <vector>

struct Scene
{
	std::vector<Primitive*> primitives;
	std::vector<Light*> lights;
};

class SceneManager
{
public:
	SceneManager();

	Scene* GetActiveScene();

private:
	Scene* activeScene;
};