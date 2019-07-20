#pragma once
#include "Timer.h"
#include <array>

class FPSCounter
{
	static int fpsArrayCurrent;
	static array<int, 10> fpsArray;
	static int fps;
	static float deltaTime;
	static Timer timer;

public:
	static void StartCounting();
	static void StopCounting();

	static int GetRawFPS();
	static int GetAverageFPS();
	static float GetDeltaTime();

private:
	FPSCounter();
};
