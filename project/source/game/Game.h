#pragma once

class Config;
class Engine;

class Game
{
public:
	Game();
	~Game();
	bool Start();

private:
	bool InitGame();

	void SetGameDefaults();
	void InitConfig();

	void LoadHiscores();
	void SaveHiscores();

	cstring GetWindowTitle();

private:
	struct Options
	{
		INT2 window_size;
		bool fullscreen;
	} options;
	Engine* engine;
	Config* config;
	string title;
};
