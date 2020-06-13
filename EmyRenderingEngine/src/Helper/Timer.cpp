#include "Timer.h"

void Timer::Start()
{
	start = chrono::high_resolution_clock::now();
	isStop = false;
}

double Timer::PeekTime() const
{
	if (isStop)
		return chrono::duration<double, milli>(end - start).count();
	else
		return chrono::duration<double, milli>(chrono::high_resolution_clock::now() - start).count();
}

double Timer::Stop()
{
	end = chrono::high_resolution_clock::now();
	isStop = true;
	return PeekTime();
}

bool Timer::IsStop() const
{
	return isStop;
}