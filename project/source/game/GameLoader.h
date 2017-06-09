#pragma once

class CGameLoader
{
public:
	CGameLoader();

	void AddError(cstring msg);
	template<typename... Args>
	void AddError(cstring msg, const Args&... args)
	{
		AddError(Format(msg, args...));
	}

	bool HaveErrors() { return errors > 0; }
	uint GetErrorCount() { return errors; }
	void FormatErrors(string& str);

private:
	vector<string> top_errors;
	uint errors;
	static const uint TOP_ERRORS = 5;
};

extern CGameLoader GameLoader;
