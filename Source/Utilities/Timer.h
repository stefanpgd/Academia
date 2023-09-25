#pragma once

#include <chrono>
#include <string>

class Timer
{
public:
	Timer(int logSize, std::string name = "Timer");

	void Start(bool printAverage = false, bool printTotalTimeLasped = false);
	void Stop();

	float Log(bool printResult = false);
	float GetAverage();
	float GetTotalLoggedTime();

private:
	std::chrono::high_resolution_clock clock;
	std::chrono::duration<long long, std::milli> t0;
	std::chrono::duration<long long, std::milli> t1;

	float* logs;
	int logSize;
	int currentLogIndex;

	bool running = false;
	bool stopped = false;
	bool logAverageToConsole = false;
	bool logTotalToConsole = false;

	std::string name;
};