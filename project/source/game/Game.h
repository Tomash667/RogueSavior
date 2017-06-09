#pragma once

#include "GameHandler.h"

class Config;
class Engine;

class Game : public GameHandler
{
public:
	Game();
	~Game();
	bool Start();

private:
	void OnCleanup() override;
	void OnTick(float dt) override;

	bool InitGame();
	void Loop();

	void SetGameDefaults();
	void InitConfig();
	void LoadConfig();
	void SaveConfig();

	void LoadHiscores();
	void SaveHiscores();

	cstring GetWindowTitle();

	struct Options
	{
		INT2 window_size;
		int window_hz;
		bool fullscreen;
	} options;
	Engine* engine;
	Config* config;
	string title;
};
