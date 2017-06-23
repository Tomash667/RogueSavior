#include "Pch.h"
#include "Core.h"
#include "Server.h"
#include <RakPeerInterface.h>

using namespace RakNet;

Server::Server() : peer(nullptr)
{
	logger = new TextLogger("server_log.txt");
}

Server::~Server()
{
	delete logger;
}

bool Server::Start()
{
	if(state != New)
		return false;

	state = Starting;
	thread = new std::thread([this] {ServerEntryPoint(); });
	return true;
}

void Server::ServerEntryPoint()
{
	if(!peer)
	{
		peer = RakPeerInterface::GetInstance();
		logger->Info("Created RakNet peer.");
	}

	SocketDescriptor sd(port, nullptr);
	sd.socketFamily = AF_INET;
	StartupResult result = peer->Startup(max_players, &sd, 1);
	if(!result != RAKNET_STARTED)
	{
		msg = Format("Failed to startup server (%d) on port %u.", result, port);
		logger->Error(msg.c_str());
		state = Failed;
		return;
	}

	peer->SetMaximumIncomingConnections(max_players);

#ifdef _DEBUG
	peer->SetTimeoutTime(1000 * 60 * 60, UNASSIGNED_SYSTEM_ADDRESS);
#endif

	logger->Info("Server created.");
	state = Lobby;

	while(true)
	{
		Sleep(0);
	}
}

void Server::Reset()
{
	assert(state == Failed);
	state = New;
	delete thread;
}

void Server::Cleanup()
{

}
