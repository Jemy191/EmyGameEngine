#include "FPSCounter.h"
#include "Log.h"

int FPSCounter::fpsArrayCurrent;
array<int, 10> FPSCounter::fpsArray;
int FPSCounter::fps = 0;
float FPSCounter::deltaTime = 1.0f;
Timer FPSCounter::timer;

void FPSCounter::StartCounting()
{
	if(timer.IsStop())
	timer.Start();
	else
	{
		Logger::Log(WARNING, "Stop the counter before starting it");
	}
}

void FPSCounter::StopCounting()
{
	deltaTime = static_cast<float>(timer.Stop()) / 1000.0f;
	fps = static_cast<int>(1.0f / deltaTime);

	fpsArray[fpsArrayCurrent] = fps;

	fpsArrayCurrent++;

	if (fpsArrayCurrent >= fpsArray.size())
		fpsArrayCurrent = 0;
}

int FPSCounter::GetRawFPS()
{
	return fps;
}
int FPSCounter::GetAverageFPS()
{
	int averageFps = 0;
	for (int fps : fpsArray)
	{
		averageFps += fps;
	}
	averageFps = averageFps / fpsArray.size();

	return averageFps;
}
float FPSCounter::GetDeltaTime()
{
	return deltaTime;
}
