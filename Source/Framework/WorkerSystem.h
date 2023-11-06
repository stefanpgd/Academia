#pragma once
#include <vector>

// Multi-threading //
#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>

class Renderer;

enum class JobState
{
	ToDo,
	Processing,
	Done
};

struct JobTile
{
	JobState State = JobState::ToDo;

	unsigned int x;
	unsigned int y;
	unsigned int xMax;
	unsigned int yMax;
};

class WorkerSystem
{
public:
	WorkerSystem(Renderer* renderer, unsigned int screenWidth, unsigned int screenHeight);

	void Update();
	void NotifyWorkers();

	void ResizeJobTiles(unsigned int screenWidth, unsigned int screenHeight);

private:
	void Work();

private:
	unsigned int screenWidth;
	unsigned int screenHeight;

	Renderer* renderer;
	bool isRunning = true;

	int threadsAvailable;
	unsigned int tileSize = 8;
	std::thread* threads;
	std::vector<JobTile> jobTiles;
	std::atomic<int> workIndex;
	std::condition_variable iterationLock;
	std::mutex rayLock;
};