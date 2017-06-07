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
	~Engine();

	bool Init(const EngineInitOptions& options);
	void ShowError(cstring msg);

private:
	Render* render;
	Window* window;
};
