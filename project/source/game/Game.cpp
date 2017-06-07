#include "Pch.h"
#include "Core.h"
#include "Config.h"
#include "Engine.h"
#include "Game.h"
#include "Version.h"

Game::Game() : config(nullptr), engine(new Engine)
{

}

Game::~Game()
{
	delete engine;
	delete config;
}

bool Game::Start()
{
	SetGameDefaults();
	InitConfig();
	config->Load();
	config->Save();

	EngineInitOptions engine_options;
	engine_options.fullscreen = options.fullscreen;
	engine_options.window_size = options.window_size;
	engine_options.window_title = GetWindowTitle();
	if(!engine->Init(engine_options))
		return false;

	if(!InitGame())
		return false;

	// go to load screen
	return true;
}

void Game::SetGameDefaults()
{
	options.window_size = INT2(1024, 768);
	options.fullscreen = false;
}

void Game::InitConfig()
{
	config = new Config("config.txt");
	config->Add("engine.window_size", options.window_size);
	config->Add("engine.fullscreen", options.fullscreen);
}

cstring Game::GetWindowTitle()
{
	if(title.empty())
	{
		title = "RogueSavior ";
		title += VERSION::STRING;
#ifdef _DEBUG
		title += " - DEBUG";
#endif
	}
	return title.c_str();
}

bool Game::InitGame()
{
	return true;
}
