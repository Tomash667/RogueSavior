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

#include "Window.h"
#include "Color.h"

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
	{
		engine->Shutdown();
		return false;
	}

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

	static int tmp;
	config->Add("test.root.left.left1", tmp);
	config->Add("test.root.left.left2", tmp);
	config->Add("test.root.right.right1", tmp);
	config->Add("test.root.right.right2", tmp);
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
		ResourceManager& res_mgr = engine->GetResourceManager();

		res_mgr.AddDir("data");

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
		
		Scene& scene = engine->GetScene();
		player = new SceneNode(res_mgr.LoadMesh("data/human.qmsh"));
		scene.Add(player);
		auto floor = new SceneNode(res_mgr.LoadMesh("data/floor.qmsh"));
		scene.Add(floor);
		auto& camera = scene.GetCamera();
		camera.mode = Camera::ThirdPersonSide;
		camera.target = player;
		camera.height = 1.7f;
		for(int i = 0; i < 3; ++i)
		{
			marker[i] = new SceneNode(res_mgr.LoadMesh("data/marker.qmsh"));
			scene.Add(marker[i]);
		}

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

	const float c_cam_angle_min = PI + 0.1f;
	const float c_cam_angle_max = PI*1.8f - 0.1f;

	auto& scene = engine->GetScene();
	auto& camera = scene.GetCamera();
	int div = 400;
	camera.rot.y += -float(Input.GetMouseMove().y) / div;
	if(camera.rot.y > c_cam_angle_max)
		camera.rot.y = c_cam_angle_max;
	if(camera.rot.y < c_cam_angle_min)
		camera.rot.y = c_cam_angle_min;
	camera.rot.x = Clip(camera.rot.x + float(Input.GetMouseMove().x) / div);

	extern VEC3 g_from, g_to, g_from2;

	marker[0]->pos = g_to;
	marker[0]->tint = VEC3(1, 0, 0);
	marker[1]->pos = g_from;
	marker[1]->tint = VEC3(0, 1, 0);
	marker[2]->pos = g_from2;
	marker[2]->tint = VEC3(0, 0, 1);

	// scroll distance
	camera.distance = Clamp(camera.distance - float(Input.GetMouseWheel()), 0.5f, 6.f);


	// rotate player to face camera
	float dif = abs(camera.rot.x - player->rot.y);
	if(dif >= 0.1f)
		player->rot.y = camera.rot.x;
	
	int movex = 0, movey = 0;
	if(Input.Down('W'))
		movey = 1;
	if(Input.Down('S'))
		movey -= 1;
	if(Input.Down('A'))
		movex = -1;
	if(Input.Down('D'))
		movex += 1;

	if(movex != 0 || movey != 0)
	{
		float speed, dir;
		if(movey == 1)
		{
			if(movex == 0)
			{
				dir = ToRadians(180.f);
				speed = 1.f;
			}
			else
			{
				speed = 0.75f;
				if(movex == -1)
					dir = ToRadians(145.f);
				else
					dir = ToRadians(215.f);
			}
		}
		else if(movey == -1)
		{
			speed = 0.25f;
			if(movex == 0)
				dir = ToRadians(0.f);
			else if(movex == -1)
				dir = ToRadians(35.f);
			else
				dir = ToRadians(325.f);
		}
		else
		{
			speed = 0.5f;
			if(movex == -1)
				dir = ToRadians(90.f);
			else
				dir = ToRadians(270.f);
		}

		dir = Clip(dir + camera.rot.x);

		VEC3 move = VEC3(sin(dir), 0, cos(dir)) * 20.f * speed * dt;
		player->pos += move;
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

cstring HISCORES_FILENAME = "hiscores.dat";

void Game::LoadHiscores()
{
	FileReader f(HISCORES_FILENAME);
	if(!f)
	{
		Info("File '%s' missing.", HISCORES_FILENAME);
		return;
	}

	char sign[4];
	if(!f.Read(sign) || memcmp(sign, "RSHI", 4) != 0)
	{
		Error("Invalid hiscores signature.");
		return;
	}

	uint count;
	if(!f.Read(count) || !f.Ensure(count * Hiscore::MIN_SIZE))
	{
		Error("Broken hiscores.");
		return;
	}

	hiscores.reserve(count);
	for(uint i = 0; i < count; ++i)
	{
		Hiscore* hi = new Hiscore;
		if(!(f >> *hi))
		{
			Error("Broken hiscores at index %u.", i);
			delete hi;
			return;
		}
		uint crc = hi->CalculateCrc();
		if(crc != hi->crc)
		{
			Error("Invalid hiscore at index %u.", i);
			delete hi;
		}
		else
			hiscores.push_back(hi);
	}

	Info("Loaded hiscores.");
}

void Game::SaveHiscores()
{
	FileWriter f(HISCORES_FILENAME);
	if(!f)
	{
		Error("Failed to open file '%s'.", HISCORES_FILENAME);
		return;
	}

	f.Write("RSHI", 4);
	f << hiscores.size();
	for(auto hi : hiscores)
		f << *hi;

	Info("Saved hiscores.");
}

bool Game::AddHiscore(Hiscore* hi)
{
	assert(hi);

	auto it = std::upper_bound(hiscores.begin(), hiscores.end(), hi, [](const Hiscore* hi1, const Hiscore* hi2)
	{
		return hi1->score > hi2->score;
	});
	if(it == hiscores.end())
	{
		if(hiscores.size() < 100u)
		{
			hiscores.push_back(hi);
			SaveHiscores();
			return true;
		}
		else
			return false;
	}
	else
	{
		hiscores.insert(it, hi);
		if(hiscores.size() > 100u)
		{
			delete hiscores.back();
			hiscores.pop_back();
		}
		SaveHiscores();
		return true;
	}
}
