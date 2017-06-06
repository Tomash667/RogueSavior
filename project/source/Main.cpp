#include "Pch.h"
#include "Core.h"
#include "Game.h"
#include "WindowsIncl.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	Game* game = new Game;
	bool ok = game->Start();
	delete game;
	return ok ? 0 : 1;
}
