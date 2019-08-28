#include "Log.h"
#include <fstream>
#if CONSOLE
#include <iostream>
#endif // CONSOLE

bool Logger::outputToFile = true;

void Logger::Log(std::string message)
{
	Log(MESSAGE, message);
}

void Logger::Log(LogSeverity severity, std::string message)
{
#if CONSOLE
	std::cout << LogSeverityToString(severity) << ": " << message << std::endl;
#endif // CONSOLE
	if (outputToFile)
	{
		//std::ofstream outputFile("Log.html");
		//outputFile << "<span class='Severity'>" << LogSeverityToString(severity) << "</span>: " << message << std::endl;
	}
}

void Logger::RegisterCallBack(std::function<void(std::string message, LogSeverity severity)> callback)
{
		Logger::callbacks.push_back(callback);
}

std::string Logger::LogSeverityToString(LogSeverity severity)
{
	switch (severity)
	{
		case MESSAGE:
			return "Message";
		case WARNING:
			return "Warning";
		case ERROR:
			return "Error";
		case FATAL_ERROR:
			return "Fatal Error";
	}
}
