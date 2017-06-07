#include "Pch.h"
#include "Core.h"
#include "Engine.h"
#include "Window.h"

Engine::Engine() : render(nullptr), window(nullptr)
{

}

Engine::~Engine()
{
	delete window;
}

bool Engine::Init(const EngineInitOptions& options)
{
	assert(!window);
	
	try
	{
		window = new Window;
		window->Init(options.window_title, options.window_size, options.fullscreen);
	}
	catch(cstring err)
	{
		ShowError(err);
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
