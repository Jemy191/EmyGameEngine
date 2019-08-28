#include "Log.h"
#include <fstream>
#include <stdexcept>
#if CONSOLE
#include <iostream>
#endif // CONSOLE

bool Logger::outputToFile = true;

void Logger::Open(std::string name)
{
	outputFile = std::ofstream(name, std::ios_base::out | std::ios_base::app);
}

void Logger::Close()
{
	outputFile.close();
}

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
		// We absolutely need to append endl because it call flush on the file stream
		outputFile << "<span class='Severity'>" << LogSeverityToString(severity) << "</span>: " << message << std::endl;
	}
	if(severity == LogSeverity::FATAL_ERROR)
		throw std::runtime_error(message);
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
