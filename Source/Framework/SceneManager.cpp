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

	vec3 white = vec3(0.83f);
	vec3 orange = vec3(1, 0.578, 0.067);
	vec3 blue = vec3(0.067, 0.698, 1);

	left->material.Color = orange;
	right->material.Color = blue;
	bottom->material.Color = white;
	back->material.Color = white;
	top->material.Color = white;

	left->material.Specularity = 0.25f;
	right->material.Specularity = 0.25f;

	Plane* light = new Plane(vec3(0.05f, 0.999f, 0.95f), vec3(0.95f, 0.999f, 0.95f), vec3(0.05f, 0.999f, 0.05f));
	light->material.isEmissive = true;

	Sphere* metal = new Sphere(vec3(0.3, 0.15f, 0.7), 0.15f);
	metal->material.Color = vec3(0.95f, 0.92f, 0.96f);
	metal->material.Specularity = 1.0f;
	metal->material.SpecularGloss = 0.955f;

	Sphere* glass2 = new Sphere(vec3(0.75, 0.165f, 0.35f), 0.165f);
	glass2->material.isDielectric = true;
	glass2->material.SpecularGloss = 0.8f;

	activeScene->primitives.push_back(bottom);
	activeScene->primitives.push_back(left);
	activeScene->primitives.push_back(right);
	activeScene->primitives.push_back(back);
	activeScene->primitives.push_back(top);
	activeScene->primitives.push_back(light);
	activeScene->primitives.push_back(glass2);
	activeScene->primitives.push_back(metal);

	Light* l1 = new Light();
	l1->Position = vec3(0.5f, 0.93f, 0.5f);
	l1->Intensity = 0.5f;
	l1->Color = vec3(1.0f);

	l1->HasArea = true;
	l1->Scale = vec3(0.1f, 0.1f, 0.1f);

	//lights.push_back(l1);

	Light* l2 = new Light();
	l2->Position = vec3(0.5f, 0.1f, 0.5f);
	l2->Intensity = 0.1f;
	l2->Color = orange;

	l2->HasArea = true;
	l2->Scale = vec3(0.1f, 0.1f, 0.1f);

	activeScene->lights.push_back(l2);

	Light* l3 = new Light();
	l3->Position = vec3(0.1f, 0.4f, 0.9f);
	l3->Intensity = 0.1f;
	l3->Color = blue;

	l3->HasArea = true;
	l3->Scale = vec3(0.1f, 0.1f, 0.1f);

	activeScene->lights.push_back(l3);

	Light* l4 = new Light();
	l4->Position = vec3(0.9f, 0.4f, 0.9f);
	l4->Intensity = 0.1f;
	l4->Color = vec3(1.0f);

	l4->HasArea = true;
	l4->Scale = vec3(0.1f, 0.1f, 0.1f);

	activeScene->lights.push_back(l4);
}

Scene* SceneManager::GetActiveScene()
{
	return activeScene;
}
