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

	Plane* bottom = new Plane(vec3(-0.35f, 0.0f, 0.0f), vec3(1.35f, 0.0f, 0.0f), vec3(-0.35f, 0.0f, 1.0f));
	Plane* left = new Plane(vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 0.0f, 1.0f), vec3(0.0f, 1.0f, 0.0f));
	Plane* right = new Plane(vec3(1.0f, 0.0f, 1.0f), vec3(1.0f, 0.0f, 0.0f), vec3(1.0f, 1.0f, 1.0f));
	Plane* back = new Plane(vec3(0.0f, 0.0f, 1.0f), vec3(1.0f, 0.0f, 1.0f), vec3(0.0f, 1.0f, 1.0f));
	Plane* top = new Plane(vec3(0.0f, 1.0f, 1.0f), vec3(1.0f, 1.0f, 1.0f), vec3(0.0f, 1.0f, 0.0f));

	vec3 white = vec3(1.0f);
	vec3 orange = vec3(1, 0.578, 0.067);
	vec3 blue = vec3(0.067, 0.698, 1);

	left->material.Color = orange;
	right->material.Color = blue;
	bottom->material.Color = white;
	back->material.Color = white;
	top->material.Color = white;

	Plane* light = new Plane(vec3(0.05f, 0.999f, 0.95f), vec3(0.95f, 0.999f, 0.95f), vec3(0.05f, 0.999f, 0.05f));
	light->material.isEmissive = true;

	Sphere* glass = new Sphere(vec3(0.0f, 0.125f, 0.35f), 0.125f);
	glass->material.isDielectric = true;
	glass->material.Color = vec3(0.985, 0.710, 0.401f);
	glass->material.IoR = 1.52f;

	Sphere* lambert = new Sphere(vec3(0.333f, 0.125f, 0.35f), 0.125f);
	lambert->material.Color = vec3(1.0f);
	lambert->material.IoR = 1.225f;

	Sphere* metal = new Sphere(vec3(0.666f, 0.125f, 0.35), 0.125f);
	metal->material.Specularity = 1.0f;
	metal->material.Metalness = 1.0f;
	metal->material.Roughness = 0.025f;
	metal->material.Color = vec3(0.716f);

	Sphere* gloss = new Sphere(vec3(1.0f, 0.125f, 0.35f), 0.125f);
	gloss->material.Color = blue;
	gloss->material.Specularity = 0.25f;
	gloss->material.Roughness = 0.375f;
	gloss->material.IoR = 1.531f;

	activeScene->primitives.push_back(bottom);
	//activeScene->primitives.push_back(left);
	//activeScene->primitives.push_back(right);
	//activeScene->primitives.push_back(back);
	//activeScene->primitives.push_back(top);
	//activeScene->primitives.push_back(light);
	activeScene->primitives.push_back(glass);
	activeScene->primitives.push_back(lambert);
	activeScene->primitives.push_back(metal);
	activeScene->primitives.push_back(gloss);
}

Scene* SceneManager::GetActiveScene()
{
	return activeScene;
}
