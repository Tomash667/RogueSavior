#pragma once

enum class NetMessage
{

};

enum class LobbyPlayerMessage
{
	HELLO,
	SAY,
	QUIT,
	START,
	STOP
};

enum class LobbyServerMessage
{
	CONNECTED,
	PLAYER_JOINING,
	PLAYER_JOINED,
	PLAYER_LEFT,
	PLAYER_SAY,
	MESSAGE
};
