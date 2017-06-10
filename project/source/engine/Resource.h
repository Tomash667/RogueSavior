#pragma once

struct Resource
{
	enum State
	{
		NOT_LOADED,
		LOADED
	};

	enum Type
	{
		MESH,
		TEXTURE
	};

	State state;
	Type type;

	virtual ~Resource() {}
};
