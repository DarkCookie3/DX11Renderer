#pragma once
#include <iostream>
#include <chrono>
#include <thread>

template <int64_t N>
using frames = std::chrono::duration<int64_t, std::ratio<1, N>>;

struct TimeTriggeredEvent
{
	float time;
	std::function<void()> event;
};

class Timer
{
public:
	void SetForCurrentFrame()
	{
		Start = std::chrono::high_resolution_clock::now();
	}
	void WaitUntilNextFrame(uint32_t targetFramerate)
	{
		while (LatestUpdate + frames<60>{1} > std::chrono::high_resolution_clock::now())
		{
			std::this_thread::yield();
		}
		UpdateDeltaTime();
	}
	void BeginCounting()
	{
		Start = std::chrono::high_resolution_clock::now();
		LatestUpdate = Start;
		deltaT = 0.0f;
	}
	float GetCounted()
	{
		float milisecDeltaT = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - Start).count();
		return milisecDeltaT / 1000.0f;
	}
	float UpdateDeltaTime()
	{
		float milisecDeltaT = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - LatestUpdate).count();
		LatestUpdate = std::chrono::high_resolution_clock::now();
		deltaT = milisecDeltaT / 1000.0f;

		UpdateTimeTriggeredEvents();

		return deltaT;
	}
	float GetDeltaTime()
	{
		return deltaT;
	}
	TimeTriggeredEvent* AddTimeTriggeredEvent(float t, std::function<void()> evnt)
	{
		return &timeTriggeredEvents.emplace_back(TimeTriggeredEvent{ t, evnt });
	}
	void UpdateTimeTriggeredEvents()
	{
		std::vector<std::list<TimeTriggeredEvent>::iterator> listToDelete;
		auto it = timeTriggeredEvents.begin();
		while (it != timeTriggeredEvents.end())
		{
			it->time -= deltaT;
			if (it->time <= 0)
			{
				it->event();
				listToDelete.push_back(it);
			}
			it++;
		}
		for (auto& it : listToDelete)
		{
			timeTriggeredEvents.erase(it);
		}
	}
private:
	float deltaT;
	std::chrono::steady_clock::time_point Start;
	std::chrono::steady_clock::time_point LatestUpdate;

	std::list<TimeTriggeredEvent> timeTriggeredEvents;
};