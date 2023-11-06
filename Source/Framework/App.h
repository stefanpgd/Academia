#pragma once
#include <string>

class Renderer;
class Editor;

class App
{
public:
	App();
	~App();
	
	void Run();

private:
	void Start();
	void Update();
	void Render();

	void LoadApplicationSettings();
	void SaveApplicationSettings();

private:
	bool runApp = true;
	std::string appName = "Academia";
	std::string appSettingsFile = "Settings/app.settings";

	Editor* editor;
	Renderer* renderer;
	
	unsigned int screenWidth;
	unsigned int screenHeight;

	friend class Editor;
};