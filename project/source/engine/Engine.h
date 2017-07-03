#pragma once

#include "Timer.h"

class GameHandler;
class Gui;
class Render;
class ResourceManager;
class Scene;
class Window;

struct EngineInitOptions
{
	GameHandler* handler;
	cstring window_title;
	INT2 window_size;
	int hz;
	bool fullscreen;
};

class Engine
{
public:
	Engine();
	~Engine();

	Gui& GetGui() { return *gui; }
	ResourceManager& GetResourceManager() { return *resource_manager; }
	Scene& GetScene() { return *scene; }
	Window& GetWindow() { return *window; }
	bool Init(const EngineInitOptions& options);
	void ShowError(cstring msg);
	void StartLoop();
	void Shutdown();

private:
	void CalculateFps(float dt);
	void Cleanup();

	GameHandler* handler;
	Render* render;
	Window* window;
	ResourceManager* resource_manager;
	Scene* scene;
	Gui* gui;
	Timer timer;
	float frame_time, fps;
	uint frames;
	bool closing, inside_loop;
};
