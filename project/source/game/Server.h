#pragma once

namespace RakNet
{
	class RakPeerInterface;
}

class Server
{
public:
	Server();
	~Server();

	bool Start();
	void ServerEntryPoint();
	void Setup(uint port, uint max_players);
	void Reset();
	void Cleanup();

private:
	enum State
	{
		New,
		Starting,
		Lobby,
		Failed
	};

	void Save();
	void Load();

	State state;
	std::thread* thread;
	RakNet::RakPeerInterface* peer;
	uint port, max_players;
	string msg;
	Logger* logger;
};
