#pragma once

//-----------------------------------------------------------------------------
// Base logger, don't log anything
class Logger
{
public:
	enum Level
	{
		L_INFO,
		L_WARN,
		L_ERROR
	};

	virtual ~Logger() {}
	virtual void Log(Level level, cstring msg, const tm& time) {};
	virtual void Flush() {}
	void Log(Level level, cstring msg);
	void LogStart();

	void Info(cstring msg)
	{
		Log(L_INFO, msg);
	}
	template<typename... Args>
	void Info(cstring msg, const Args&... args)
	{
		Log(L_INFO, Format(msg, args...));
	}
	void Warn(cstring msg)
	{
		Log(L_WARN, msg);
	}
	template<typename... Args>
	void Warn(cstring msg, const Args&... args)
	{
		Log(L_WARN, Format(msg, args...));
	}
	void Error(cstring msg)
	{
		Log(L_ERROR, msg);
	}
	template<typename... Args>
	void Error(cstring msg, const Args&... args)
	{
		Log(L_ERROR, Format(msg, args...));
	}

protected:
	static const cstring level_names[3];
};

//-----------------------------------------------------------------------------
// Pre logger, used before deciding where log to
class PreLogger : public Logger
{
	struct Prelog
	{
		Level level;
		string msg;
		tm time;
	};

	vector<Prelog*> prelogs;
	bool flush;

public:
	PreLogger() : flush(false) {}
	~PreLogger();
	void Apply(Logger* logger);
	void Log(Level level, cstring msg, const tm& time) override;
	void Flush() override;
};

//-----------------------------------------------------------------------------
// Logger to multiple loggers
class MultiLogger : public Logger
{
	vector<Logger*> loggers;

public:
	~MultiLogger();
	void Add(Logger* logger);
	void Remove(Logger* logger);
	void Log(Level level, cstring msg, const tm& time) override;
	void Flush() override;
};

//-----------------------------------------------------------------------------
// Logger to system console window
class ConsoleLogger : public Logger
{
public:
	ConsoleLogger();
	void Log(Level level, cstring msg, const tm& time) override;
};

//-----------------------------------------------------------------------------
// Logger to text file
class TextLogger : public Logger
{
	std::ofstream f;

public:
	explicit TextLogger(cstring filename);
	~TextLogger();
	void Log(Level level, cstring msg, const tm& time) override;
	void Flush() override;
};

//-----------------------------------------------------------------------------
// Helper global functions
extern Logger* g_logger;

inline void Info(cstring msg)
{
	g_logger->Log(Logger::L_INFO, msg);
}
template<typename... Args>
inline void Info(cstring msg, const Args&... args)
{
	g_logger->Log(Logger::L_INFO, Format(msg, args...));
}

inline void Warn(cstring msg)
{
	g_logger->Log(Logger::L_WARN, msg);
}
template<typename... Args>
inline void Warn(cstring msg, const Args&... args)
{
	g_logger->Log(Logger::L_WARN, Format(msg, args...));
}

inline void Error(cstring msg)
{
	g_logger->Log(Logger::L_ERROR, msg);
}
template<typename... Args>
inline void Error(cstring msg, const Args&... args)
{
	g_logger->Log(Logger::L_ERROR, Format(msg, args...));
}
