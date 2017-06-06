#pragma once

class Window;

class Engine
{
public:
	Engine();

	bool InitEngine();

	Window& GetWindow() { return *window; }

private:
	Window* window;
};
