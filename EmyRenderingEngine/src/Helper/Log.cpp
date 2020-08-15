#include "Helper/Log.h"
#include <stdexcept>
#include <filesystem>
#if CONSOLE
#include <iostream>
#endif // CONSOLE

bool Logger::outputToFile = true;
std::ofstream Logger::outputFile;
std::vector<std::function<void(std::string message, LogSeverity severity)>> Logger::callbacks;

void Logger::Open(std::string name)
{
	outputFile = std::ofstream(name + ".html", std::ios_base::out);
	outputFile << "<head>" << std::endl;
	outputFile << "<link rel = 'stylesheet' href = 'Tool/LogStyle.css'>" << std::endl;
	outputFile << "</head>" << std::endl;

	Log("Logger Open.");

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
		outputFile << "<span class='" << LogSeverityToString(severity) << "'>" << LogSeverityToString(severity) << "</span>: " << message << "</br>" << std::endl;
	}
	if (severity == LogSeverity::FATAL_ERROR)
		throw std::runtime_error(message);
}

void Logger::RegisterCallBack(std::function<void(std::string message, LogSeverity severity)> callback)
{
	Logger::callbacks.push_back(callback);
	Log("Logger callback registered.");
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
			return "Fatal_Error";
	}
}
