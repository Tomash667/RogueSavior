#pragma once

#include "Crc.h"
#include "GameHandler.h"

class Config;
class Engine;
struct SceneNode;

struct Hiscore
{
	static const uint MIN_SIZE = 20;
	string name, killed_by;
	uint level, days, score, crc;

	friend bool operator >> (FileReader& f, Hiscore& hi)
	{
		f >> hi.name;
		f >> hi.killed_by;
		f >> hi.level;
		f >> hi.days;
		f >> hi.score;
		f >> hi.crc;
		return f;
	}

	friend void operator << (FileWriter& f, const Hiscore& hi)
	{
		f << hi.name;
		f << hi.killed_by;
		f << hi.level;
		f << hi.days;
		f << hi.score;
		f << hi.crc;
	}

	uint CalculateCrc() const
	{
		Crc crc;
		crc.Update(name);
		crc.Update(killed_by);
		crc.Update(level);
		crc.Update(days);
		crc.Update(score);
		return crc;
	}
};

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
	bool AddHiscore(Hiscore* hi);

	cstring GetWindowTitle();

	void ConfigureLogger();

	struct Options
	{
		INT2 window_size;
		int window_hz;
		bool fullscreen;
		string log_file;
		bool log_console;
	} options;
	Engine* engine;
	Config* config;
	string title, compile_time;
	SceneNode* player;
	SceneNode* marker[3];

	vector<Hiscore*> hiscores;
};
