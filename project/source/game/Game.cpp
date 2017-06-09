#include "Pch.h"
#include "Core.h"
#include "Config.h"
#include "Engine.h"
#include "Game.h"
#include "GameLoader.h"
#include "Input.h"
#include "Item.h"
#include "ResourceManager.h"
#include "Scene.h"
#include "SceneNode.h"
#include "UnitData.h"
#include "Version.h"

Game::Game() : config(nullptr), engine(new Engine)
{
	compile_time = GetCompileTime();
	g_logger = new PreLogger;
	g_logger->LogStart();
	Info(GetWindowTitle());
	Info(Format("Compile time: %s", compile_time.c_str()));
}

Game::~Game()
{
	delete engine;
	delete config;

	g_logger->Flush();
	delete g_logger;
}

bool Game::Start()
{
	Info("Game start");

	SetGameDefaults();
	InitConfig();
	LoadConfig();
	SaveConfig();

	ConfigureLogger();

	EngineInitOptions engine_options;
	engine_options.handler = this;
	engine_options.fullscreen = options.fullscreen;
	engine_options.window_size = options.window_size;
	engine_options.window_title = GetWindowTitle();
	if(!engine->Init(engine_options))
		return false;

	if(!InitGame())
	{
		engine->Shutdown();
		return false;
	}

	engine->StartLoop();
	return true;
}

void Game::SetGameDefaults()
{
	options.window_size = INT2(1024, 768);
	options.window_hz = 0;
	options.fullscreen = false;
	options.log_file = "log.txt";
	options.log_console = false;
}

void Game::InitConfig()
{
	config = new Config("config.txt");
	config->Add("engine.window_size", options.window_size);
	config->Add("engine.window_hz", options.window_hz);
	config->Add("engine.fullscreen", options.fullscreen);
	config->Add("log_file", options.log_file);
	config->Add("log_console", options.log_console);
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

		if(GameLoader.HaveErrors())
		{
			string err = Format("Failed to initialize game. %u errors found, check log for more details.", GameLoader.GetErrorCount());
			Error(err.c_str());
			err += '\n';
			GameLoader.FormatErrors(err);
			engine->ShowError(err.c_str());
			return false;
		}
		
		SceneNode* node = new SceneNode;
		node->mesh = engine->GetResourceManager().LoadMesh("data/human.mesh");
		node->pos = VEC3(0, 0, 0);
		engine->GetScene().Add(node);

		return true;
	}
	catch(cstring err)
	{
		cstring err_msg = Format("Failed to initialize game.\n%s", err);
		Error(err_msg);
		engine->ShowError(err_msg);
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
	Info("Cleaning game.");
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

void Game::ConfigureLogger()
{
	bool log_console = options.log_console;
#ifdef _DEBUG
	log_console = true;
#endif

	auto prelogger = (PreLogger*)g_logger;

	if(!options.log_file.empty() && log_console)
	{
		auto multilogger = new MultiLogger;
		multilogger->Add(new TextLogger(options.log_file.c_str()));
		multilogger->Add(new ConsoleLogger);
		g_logger = multilogger;
	}
	else if(!options.log_file.empty())
		g_logger = new TextLogger(options.log_file.c_str());
	else if(log_console)
		g_logger = new ConsoleLogger;
	else
		g_logger = new Logger;
	
	prelogger->Apply(g_logger);
	delete prelogger;
}
