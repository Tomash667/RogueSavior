#pragma once

#include "Engine.h"

class Config;

class Game : Engine
{
public:
	Game();
	bool Start();

private:
	bool InitGame();

	void SetGameDefaults();
	void InitConfig();

	struct Options
	{
		INT2 window_size;
		bool fullscreen;
	} options;
	Config* config;
};
