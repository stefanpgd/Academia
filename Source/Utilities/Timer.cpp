#include "Timer.h"
#include "LogHelper.h"

Timer::Timer(int logSize, std::string name) : logSize(logSize), name(name)
{
	logs = new float[logSize];
}

void Timer::Start(bool printAverage, bool printTotalTimeLasped)
{
	memset(logs, 0.0f, sizeof(float) * logSize);

	currentLogIndex = 0;
	logAverageToConsole = printAverage;
	logTotalToConsole = printTotalTimeLasped;

	t0 = std::chrono::time_point_cast<std::chrono::milliseconds>((clock.now())).time_since_epoch();
	running = true;
}

void Timer::Stop()
{
	stopped = false;
}

float Timer::Log(bool printResult)
{
	if(!running)
	{
		LOG(Log::MessageType::Debug, "Trying to Log a timer that hasn't started yet")
			return -1.0f;
	}

	if(stopped)
	{
		return 0.0f;
	}

	t1 = std::chrono::time_point_cast<std::chrono::milliseconds>((clock.now())).time_since_epoch();
	float timeElapsed = (t1 - t0).count() * .001;
	logs[currentLogIndex] = timeElapsed;
	t0 = t1;
	currentLogIndex++;

	if(printResult)
	{
		std::string msg = name + " - ";
		msg += std::to_string(timeElapsed);
		LOG(msg);
	}

	if(currentLogIndex >= logSize)
	{
		currentLogIndex = 0;

		if(logAverageToConsole)
		{
			float average = GetAverage();

			std::string msg = name + " - Average: ";
			msg += std::to_string(average);
			LOG(msg);
		}

		if(logTotalToConsole)
		{
			float total = GetTotalLoggedTime();
			std::string msg = name + " - Total: ";
			msg += std::to_string(total);
			LOG(msg);
		}
	}
}

float Timer::GetAverage()
{
	float totalTime = 0.0f;
	for(int i = 0; i < logSize; i++)
	{
		totalTime += logs[i];
	}

	totalTime /= logSize;

	return totalTime;
}

float Timer::GetTotalLoggedTime()
{
	float totalTime = 0.0f;
	for(int i = 0; i < logSize; i++)
	{
		totalTime += logs[i];
	}

	return totalTime;
}