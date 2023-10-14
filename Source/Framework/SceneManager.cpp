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

	Plane* bottom = new Plane(vec3(0.0f, 0.0f, 0.0f), vec3(1.0f, 0.0f, 0.0f), vec3(0.0f, 0.0f, 1.0f));
	Plane* left = new Plane(vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 0.0f, 1.0f), vec3(0.0f, 1.0f, 0.0f));
	Plane* right = new Plane(vec3(1.0f, 0.0f, 1.0f), vec3(1.0f, 0.0f, 0.0f), vec3(1.0f, 1.0f, 1.0f));
	Plane* back = new Plane(vec3(0.0f, 0.0f, 1.0f), vec3(1.0f, 0.0f, 1.0f), vec3(0.0f, 1.0f, 1.0f));
	Plane* top = new Plane(vec3(0.0f, 1.0f, 1.0f), vec3(1.0f, 1.0f, 1.0f), vec3(0.0f, 1.0f, 0.0f));

	//vec3 white = vec3(0.83f);
	vec3 white = vec3(0.43f);
	vec3 orange = vec3(1, 0.578, 0.067);
	vec3 blue = vec3(0.067, 0.698, 1);

	left->material.Color = orange;
	right->material.Color = blue;
	bottom->material.Color = white;
	back->material.Color = white;
	top->material.Color = white;

	Plane* light = new Plane(vec3(0.05f, 0.999f, 0.95f), vec3(0.95f, 0.999f, 0.95f), vec3(0.05f, 0.999f, 0.05f));
	light->material.isEmissive = true;

	Sphere* mirror = new Sphere(vec3(0.25, 0.1f, 0.35), 0.1f);
	mirror->material.Color = vec3(0.95f, 0.92f, 0.96f);
	mirror->material.Specularity = 1.0f;

	Sphere* lambert = new Sphere(vec3(0.5, 0.1f, 0.35f), 0.1f);
	//lambert->material.Color = orange;
	lambert->material.IoR = 1.25f;

	Sphere* gloss = new Sphere(vec3(0.75, 0.1f, 0.35f), 0.1f);
	gloss->material.Color = blue;
	gloss->material.Specularity = 0.15f;
	gloss->material.Fuzz = 0.25f;
	gloss->material.IoR = 1.53f;

	activeScene->primitives.push_back(bottom);
	//activeScene->primitives.push_back(left);
	//activeScene->primitives.push_back(right);
	activeScene->primitives.push_back(back);
	//activeScene->primitives.push_back(top);
	activeScene->primitives.push_back(light);
	activeScene->primitives.push_back(mirror);
	activeScene->primitives.push_back(lambert);
	activeScene->primitives.push_back(gloss);
}

Scene* SceneManager::GetActiveScene()
{
	return activeScene;
}
