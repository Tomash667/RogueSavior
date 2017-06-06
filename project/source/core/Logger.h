#pragma once

class Logger
{
public:
};

extern Logger* logger;

void Info(cstring msg, ...);
void Warn(cstring msg, ...);
void Error(cstring msg, ...);
