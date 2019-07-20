#pragma once
#include <chrono>

using namespace std;

class Timer
{
public:


private:
	chrono::steady_clock::time_point start;
	chrono::steady_clock::time_point end;
	bool isStop = true;

public:
	void Start();

	// Donne de temps ou est rendu le Chronom�tre.
	double PeekTime() const;

	// Donne de temps ou est rendu le Chronom�tre et l'arr�te.
	double Stop();

	bool IsStop() const;
};