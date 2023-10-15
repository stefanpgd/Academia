#include "SceneManager.h"

// Temporary until we've scene loading
// Primitives //
#include "Graphics/Sphere.h"
#include "Graphics/Plane.h"
#include "Graphics/PlaneInfinite.h"
#include "Graphics/Triangle.h"

SceneManager::SceneManager()
{
	activeScene = new Scene();

	Plane* ground = new Plane(vec3(-0.35f, 0.0f, 0.0f), vec3(1.35f, 0.0f, 0.0f), vec3(-0.35f, 0.0f, 1.0f));

	vec3 orange = vec3(1, 0.578, 0.067);
	vec3 blue = vec3(0.333, 0.785, 1);

	ground->material.Color = vec3(0.5f);

	Plane* light = new Plane(vec3(0.05f, 0.999f, 0.95f), vec3(0.95f, 0.999f, 0.95f), vec3(0.05f, 0.999f, 0.05f));
	light->material.isEmissive = true;

	Sphere* glass = new Sphere(vec3(0.0f, 0.125f, 0.35f), 0.125f);
	glass->material.isDielectric = true;
	glass->material.IoR = 1.52f;

	Sphere* lambert = new Sphere(vec3(0.333f, 0.125f, 0.35f), 0.125f);
	lambert->material.IoR = 1.125f;

	Sphere* metal = new Sphere(vec3(0.666f, 0.125f, 0.35), 0.125f);
	metal->material.Specularity = 1.0f;
	metal->material.Metalness = 1.0f;

	Sphere* gloss = new Sphere(vec3(1.0f, 0.125f, 0.35f), 0.125f);
	gloss->material.Color = blue;
	gloss->material.Specularity = 0.0f;
	gloss->material.Roughness = 0.1f;
	gloss->material.IoR = 1.531f;

	activeScene->primitives.push_back(ground);
	activeScene->primitives.push_back(glass);
	activeScene->primitives.push_back(lambert);
	activeScene->primitives.push_back(metal);
	activeScene->primitives.push_back(gloss);
}

Scene* SceneManager::GetActiveScene()
{
	return activeScene;
}
