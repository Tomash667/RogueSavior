#include "Pch.h"
#include "Core.h"
#include "GameLoader.h"

CGameLoader GameLoader;

CGameLoader::CGameLoader() : errors(0)
{

}

void CGameLoader::AddError(cstring msg)
{
	assert(msg);
	Error(msg);
	if(top_errors.size() < TOP_ERRORS)
		top_errors.push_back(msg);
	++errors;
}

void CGameLoader::FormatErrors(string& str)
{
	bool first = true;
	for(auto& e : top_errors)
	{
		if(first)
			first = false;
		else
			str += '\n';
		str += e;
	}
}
