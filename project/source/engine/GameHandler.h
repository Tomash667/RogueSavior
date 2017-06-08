#pragma once

class GameHandler
{
public:
	virtual void OnCleanup() = 0;
	virtual void OnTick(float dt) = 0;
};
