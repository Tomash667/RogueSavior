#include "Pch.h"
#include "Core.h"
#include "Config.h"
#include "Game.h"

Game::Game() : config(nullptr)
{

}

bool Game::Start()
{
	SetGameDefaults();
	InitConfig();
	config->Load();
	config->Save();

	if(!InitEngine())
		return false;

	if(!InitGame())
		return false;
	// init game

	// go to load screen
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
