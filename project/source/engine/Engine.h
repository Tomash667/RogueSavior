#pragma once

class Render;
class Window;

struct EngineInitOptions
{
	cstring window_title;
	INT2 window_size;
	bool fullscreen;
};

class Engine
{
public:
	Engine();

	bool InitEngine(const EngineInitOptions& options);

private:
	Render* render;
	Window* window;
};
