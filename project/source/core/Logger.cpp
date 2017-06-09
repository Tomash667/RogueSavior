#include "Pch.h"
#include "Core.h"
#include "WindowsIncl.h"
#include <ctime>
#include <iostream>

Logger* g_logger;
const cstring Logger::level_names[3] = {
	"INFO ",
	"WARN ",
	"ERROR"
};

void Logger::Log(Level level, cstring msg)
{
	time_t t = time(0);
	tm tm;
	localtime_s(&tm, &t);
	Log(level, msg, tm);
}

void Logger::LogStart()
{
	time_t t = time(0);
	tm t2;
	localtime_s(&t2, &t);
	Log(L_INFO, Format("Date: %04d-%02d-%02d", t2.tm_year + 1900, t2.tm_mon + 1, t2.tm_mday));
}

PreLogger::~PreLogger()
{
	DeleteElements(prelogs);
}

void PreLogger::Apply(Logger* logger)
{
	assert(logger);
	for(auto p : prelogs)
		logger->Log(p->level, p->msg.c_str(), p->time);
	if(flush)
		logger->Flush();
}

void PreLogger::Log(Level level, cstring msg, const tm& time)
{
	Prelog* p = new Prelog;
	p->level = level;
	p->msg = msg;
	p->time = time;
	prelogs.push_back(p);
}

void PreLogger::Flush()
{
	flush = true;
}

MultiLogger::~MultiLogger()
{
	DeleteElements(loggers);
}

void MultiLogger::Add(Logger* logger)
{
	assert(logger);
	loggers.push_back(logger);
}

void MultiLogger::Remove(Logger* logger)
{
	assert(logger);
	RemoveElement(loggers, logger);
}

void MultiLogger::Log(Level level, cstring msg, const tm& time)
{
	for(auto logger : loggers)
		logger->Log(level, msg, time);
}

void MultiLogger::Flush()
{
	for(auto logger : loggers)
		logger->Flush();
}

ConsoleLogger::ConsoleLogger()
{
	AllocConsole();
	FILE* file;
	freopen_s(&file, "CONOUT$", "w", stdout);
	std::cout.clear();
}

void ConsoleLogger::Log(Level level, cstring msg, const tm& time)
{
	printf("%02d:%02d:%02d %s - %s\n", time.tm_hour, time.tm_min, time.tm_sec, level_names[level], msg);
	fflush(stdout);
}

TextLogger::TextLogger(cstring filename)
{
	assert(filename);
	f.open(filename);
}

TextLogger::~TextLogger()
{
	Logger::Log(L_INFO, "*** End of log ***");
}

void TextLogger::Log(Level level, cstring msg, const tm& time)
{
	f << Format("%02d:%02d:%02d %s - %s\n", time.tm_hour, time.tm_min, time.tm_sec, level_names[level], msg);
}

void TextLogger::Flush()
{
	f.flush();
}
