#include "Pch.h"
#include "Core.h"
#include "Config.h"
#include "Engine.h"
#include "Game.h"
#include "GameLoader.h"
#include "Gui.h"
#include "Input.h"
#include "Item.h"
#include "Player.h"
#include "ResourceManager.h"
#include "Scene.h"
#include "SceneNode.h"
#include "Sprite.h"
#include "UnitData.h"
#include "Version.h"

Game* Game::_game;

Game::Game() : config(nullptr), engine(new Engine)
{
	_game = this;
	compile_time = GetCompileTime();
	g_logger = new PreLogger;
	g_logger->LogStart();
	Info(GetWindowTitle());
	Info(Format("Compile time: %s", compile_time.c_str()));
}

Game::~Game()
{
	DeleteElements(items);
	delete player->unit;
	delete player;

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

		// player
		player = new Player;
		player->unit = new Unit;
		player->unit->node = new SceneNode(res_mgr.LoadMesh("data/human.qmsh"));
		scene.Add(player->unit->node);

		// floor
		auto floor = new SceneNode(res_mgr.LoadMesh("data/floor.qmsh"));
		scene.Add(floor);

		// camera
		auto& camera = scene.GetCamera();
		camera.mode = Camera::ThirdPerson;
		camera.target = player->unit->node;
		camera.height = 1.7f;
		camera.shift = VEC2(0.5f, 0.f);

		// test ground item
		GroundItem* item = new GroundItem;
		item->item = GetItem("handgun");
		item->ammo_count = 8;
		item->node = new SceneNode(res_mgr.LoadMesh("data/bag.qmsh"));
		item->node->pos = VEC3(3, 0, 0);
		scene.Add(item->node);
		items.push_back(item);

		// gui
		auto sprite = new Sprite(res_mgr.LoadTexture("data/dot.png"));
		sprite->SetPos(VEC2(0.5f - 0.01f, 0.5f - 0.01f));
		sprite->SetSize(VEC2(0.2f, 0.2f));
		engine->GetGui().Add(sprite);

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

	player->Update(dt);
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

Camera& Game::GetCamera()
{
	return engine->GetScene().GetCamera();
}

void Game::SaveGame(FileWriter& f)
{
	f.Write("RSSV", 4);
	f.WriteCasted<byte>(0); // version

	f << items.size();
	for(auto item : items)
	{
		f << item->node->pos;
		f << item->item->id;
		f << item->ammo_count;
	}
}

bool Game::LoadGame(FileReader& f)
{
	char sign[4];
	if(!f.Read(sign, 4) || memcmp(sign, "RSSV", 4) != 0)
	{
		Error("Invalid file signature.");
		return false;
	}

	byte version;
	if(!(f >> version) || version != 0)
	{
		Error("Invalid version %u (supported 0).", version);
		return false;
	}

	return true;
}
