#include "Pch.h"
#include "Core.h"
#include "Engine.h"
#include "GameHandler.h"
#include "Input.h"
#include "Window.h"

Engine::Engine() : render(nullptr), window(nullptr), closing(false)
{

}

Engine::~Engine()
{
	delete window;
}

bool Engine::Init(const EngineInitOptions& options)
{
	assert(!window);
	assert(options.handler);

	handler = options.handler;
	
	try
	{
		window = new Window;
		window->Init(options.window_title, options.window_size, options.fullscreen);
	}
	catch(cstring err)
	{
		ShowError(Format("Failed to initialize engine.\n%s", err));
		return false;
	}

	return true;
}

void Engine::ShowError(cstring msg)
{
	// turn of fullscreen

	// hide window
	//if(window)
	//	window->Hide();

	// show message
	window->ShowError(msg);
}

void Engine::StartLoop()
{
	timer.Start();
	frames = 0;
	frame_time = 0.f;
	fps = 0.f;

	while(window->HandleMessages())
	{
		const float dt = timer.Tick();

		CalculateFps(dt);

		handler->OnTick(dt);
		if(closing)
			break;

		Input.Update();
	}

	Cleanup();
}

void Engine::CalculateFps(float dt)
{
	frames++;
	frame_time += dt;
	if(frame_time >= 1.f)
	{
		fps = frames / frame_time;
		frames = 0;
		frame_time = 0.f;
	}
}

void Engine::Cleanup()
{
	handler->OnCleanup();
}

void Engine::Shutdown()
{
	closing = true;
}
