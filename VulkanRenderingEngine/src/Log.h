#pragma once
#include <string>
#include <functional>
#include <vector>

enum LogSeverity
{
	MESSAGE,
	WARNING,
	ERROR,
	FATAL_ERROR
};

class Logger
{
public:
	static bool outputToFile;

private:
	static std::ofstream outputFile;
	static std::vector<std::function<void(std::string message, LogSeverity severity)>> callbacks;

public:
	static void Open(std::string name = "Log.html");
	static void Close();

	static void Log(std::string message);
	static void Log(LogSeverity severity, std::string message);

	static void RegisterCallBack(std::function<void(std::string message, LogSeverity severity)> callback);

private:
	static std::string LogSeverityToString(LogSeverity severity);

};