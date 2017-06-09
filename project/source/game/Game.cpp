#include "Pch.h"
#include "Core.h"
#include "Config.h"
#include "Engine.h"
#include "Game.h"
#include "Input.h"
#include "Item.h"
#include "UnitData.h"
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
	Info("Game start");

	SetGameDefaults();
	InitConfig();
	LoadConfig();
	SaveConfig();

	EngineInitOptions engine_options;
	engine_options.handler = this;
	engine_options.fullscreen = options.fullscreen;
	engine_options.window_size = options.window_size;
	engine_options.window_title = GetWindowTitle();
	if(!engine->Init(engine_options))
		return false;

	if(!InitGame())
		return false;

	engine->StartLoop();
	return true;
}

void Game::SetGameDefaults()
{
	options.window_size = INT2(1024, 768);
	options.window_hz = 0;
	options.fullscreen = false;
}

void Game::InitConfig()
{
	config = new Config("config.txt");
	config->Add("engine.window_size", options.window_size);
	config->Add("engine.window_hz", options.window_hz);
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
	Info("Initializing game.");

	try
	{
		SetItemPointers();
		return true;
	}
	catch(cstring err)
	{
		engine->ShowError(Format("Failed to initialize game.\n%s", err));
		return false;
	}
}

void Game::OnTick(float dt)
{
	if(Input.Down(K_ALT) && Input.Pressed(K_F4))
	{
		engine->Shutdown();
		return;
	}
}

void Game::OnCleanup()
{
	CleanupUnitDatas();
	CleanupItems();
}

void Game::LoadConfig()
{
	Info("Loading config.");
	config->Load();

	if(options.window_size.x < 800)
		options.window_size.x = 800;
	if(options.window_size.y < 600)
		options.window_size.y = 600;
}

void Game::SaveConfig()
{
	Info("Saving config.");
	config->Save();
}
