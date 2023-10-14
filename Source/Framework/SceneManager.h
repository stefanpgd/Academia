#pragma once
#include "Graphics/Primitive.h"
#include <vector>

struct Scene
{
	std::vector<Primitive*> primitives;
};

class SceneManager
{
public:
	SceneManager();

	Scene* GetActiveScene();

private:
	Scene* activeScene;
};