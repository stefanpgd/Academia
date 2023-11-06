#include "WorkerSystem.h"
#include "Framework/Renderer.h"
#include "Graphics/RayTracer.h"

#include "Utilities/Utilities.h"

WorkerSystem::WorkerSystem(Renderer* renderer, unsigned int screenWidth, unsigned int screenHeight) :
	renderer(renderer), screenWidth(screenWidth), screenHeight(screenHeight)
{
	LOG("Retrieving thread count...");
	threadsAvailable = std::thread::hardware_concurrency();

	//threadsAvailable = std::thread::hardware_concurrency() - 1;
	threads = new std::thread[threadsAvailable];
	LOG("There are: '" + std::to_string(threadsAvailable) + "' threads available for use.");

	ResizeJobTiles(screenWidth, screenHeight);
	workIndex.store(jobTiles.size() - 1);

	for(int i = 0; i < threadsAvailable; i++)
	{
		threads[i] = std::thread([this] { Work(); });
	}

	LOG("Multi-threading succesfully started.");
}

void WorkerSystem::Update()
{
	if(workIndex < 0)
	{
		// All jobs have been picked up, check if they are done as well
		// if so, that means an trace iteration has been completed, and we can
		// update the screen buffer
		bool iterationDone = true;

		for(unsigned int i = 0; i < jobTiles.size(); i++)
		{
			if(jobTiles[i].State != JobState::Done)
			{
				iterationDone = false;
			}
		}

		if(iterationDone)
		{
			renderer->updateScreenBuffer = true;
		}
	}
}

void WorkerSystem::NotifyWorkers()
{
	for(unsigned int i = 0; i < jobTiles.size(); i++)
	{
		jobTiles[i].State = JobState::ToDo;
	}

	workIndex.store(jobTiles.size() - 1);
	iterationLock.notify_all();
}

void WorkerSystem::ResizeJobTiles(unsigned int screenWidth, unsigned int screenHeight)
{
	this->screenWidth = screenWidth;
	this->screenHeight = screenHeight;

	jobTiles.clear();

	int horizontalTiles = screenWidth / tileSize;
	int verticalTiles = screenHeight / tileSize;

	for(unsigned int y = 0; y < verticalTiles; y++)
	{
		for(unsigned int x = 0; x < horizontalTiles; x++)
		{
			JobTile tile;
			tile.x = x * tileSize;
			tile.y = y * tileSize;
			tile.xMax = x * tileSize + tileSize;
			tile.yMax = y * tileSize + tileSize;

			jobTiles.push_back(tile);
		}
	}
}

void WorkerSystem::Work()
{
	while(isRunning)
	{
		// Retrieve job tile index
		int index = workIndex.fetch_sub(1);

		// All jobs available are either in process or have been completed
		// Wait until you receive a signal to continue again.
		if(index < 0)
		{
			std::unique_lock<std::mutex> lock(rayLock);
			iterationLock.wait(lock);
			continue;
		}

		JobTile& tile = jobTiles[index];

		if(tile.State != JobState::ToDo)
		{
			continue;
		}

		tile.State = JobState::Processing;

		for(int x = tile.x; x < tile.xMax; x++)
		{
			for(int y = tile.y; y < tile.yMax; y++)
			{
				int i = x + y * screenWidth;
				renderer->sampleBuffer[i] += renderer->rayTracer->Trace(x, y);
			}
		}

		tile.State = JobState::Done;
	}
}