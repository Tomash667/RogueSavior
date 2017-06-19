#include "Pch.h"
#include "Core.h"
#include "Engine.h"
#include "GameHandler.h"
#include "Input.h"
#include "Render.h"
#include "ResourceManager.h"
#include "Scene.h"
#include "Window.h"

Engine::Engine() : render(nullptr), resource_manager(nullptr), scene(nullptr), window(nullptr), closing(false), inside_loop(false)
{

}

Engine::~Engine()
{
	delete scene;
	delete resource_manager;
	delete render;
	delete window;
}

bool Engine::Init(const EngineInitOptions& options)
{
	assert(!window);
	assert(options.handler);

	Info("Initializing engine.");

	handler = options.handler;
	
	try
	{
		window = new Window;
		window->Init(options.window_title, options.window_size, options.fullscreen);

		render = new Render;
		render->Init(window->GetHandle(), options.window_size, options.hz, options.fullscreen);

		resource_manager = new ResourceManager;
		resource_manager->Init(render->GetDevice());

		scene = new Scene;
		scene->Init(render);
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
	try
	{
		timer.Start();
		frames = 0;
		frame_time = 0.f;
		fps = 0.f;
		inside_loop = true;

		while(window->Update())
		{
			const float dt = timer.Tick();

			CalculateFps(dt);
			Input.UpdateShortcuts();

			handler->OnTick(dt);
			if(closing)
				break;

			scene->Update(dt);
			render->Draw();
			Input.Update();
		}
	}
	catch(cstring err)
	{
		ShowError(err);
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
	Info("Shutting down engine.");
	if(inside_loop)
		closing = true;
	else
		Cleanup();
}
