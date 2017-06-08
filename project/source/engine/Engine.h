#pragma once

#include "Timer.h"

class GameHandler;
class Render;
class Window;

struct EngineInitOptions
{
	GameHandler* handler;
	cstring window_title;
	INT2 window_size;
	bool fullscreen;
};

class Engine
{
public:
	Engine();
	~Engine();

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
	Timer timer;
	float frame_time, fps;
	uint frames;
	bool closing;
};
