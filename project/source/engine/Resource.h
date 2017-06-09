#pragma once

class Resource
{
public:
	enum State
	{
		NOT_LOADED,
		LOADED
	};

private:
	State state;
};
