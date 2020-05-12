#pragma once

#define TICKS_PER_SECOND 60

#include <pch.hpp>
#include <chrono>
#include <iostream>

#include <vulkan/loop/TickTask.hpp>
#include <thread>
#include <mutex>

using namespace std::chrono;

class cGameLoop
{
private:
    // Amount of milliseconds per tick
    const duration<uint, std::milli> tPERIOD = milliseconds((int) round((1.0f / TICKS_PER_SECOND) * 1000.0f));

    // Tasks to run every tick
    std::vector<iTickTask*> papTasks;
    std::mutex poTasksMutex;

    // If false, the loop will terminate
    bool pbRunning;

public:
    // Destroys all the tasks inside this loop
    ~cGameLoop();

    // Starts this GameLoop
    void operator()();

    // Add a task to run every tick
    void AddTask(iTickTask* pTask);

    // Stop this GameLoop
    void Stop();

private:
    // Main loop handling the timing
    void MainLoop();

    // Runs all the tick tasks
    void Tick();

    // Print an error if the duration of the previous tick exceeds a threshold
    void CheckTickTime(time_point<steady_clock> tPrev, time_point<steady_clock> tNow);
};

cGameLoop::~cGameLoop()
{
    // Delete all the tick tasks
    for (iTickTask* pTask : papTasks)
    {
        delete pTask;
    }
}

void cGameLoop::operator()()
{
    pbRunning = true;
    MainLoop();
}

void cGameLoop::AddTask(iTickTask* pTask)
{
    // Lock the mutex before adding the task
    poTasksMutex.lock();
    papTasks.push_back(pTask);
    poTasksMutex.unlock();
}

void cGameLoop::Stop()
{
    pbRunning = false;
}

void cGameLoop::MainLoop()
{
    // tNext is the time at which the next tick should start
    time_point tNext = steady_clock::now();

    // tPrev is the time when the last tick started
    time_point tPrev = tNext - tPERIOD;

    while (pbRunning)
    {
        // Run tick time checks
        time_point tNow = steady_clock::now();
        CheckTickTime(tPrev, tNow);
        tPrev = tNow;

        // Run all the tick tasks
        Tick();

        // The next tick should start PERIOD milliseconds after the last one
        tNext += tPERIOD;
        std::this_thread::sleep_until(tNext);
    }
}

void cGameLoop::CheckTickTime(time_point<steady_clock> tPrev, time_point<steady_clock> tNow)
{
    const uint uiPERIOD_COUNT = tPERIOD.count();

    // Calculate the time this tick took
    uint uiTickTime = round<milliseconds>(tNow - tPrev).count();

    // If the time exceeds a threshold, print a warning
    if (uiTickTime > uiPERIOD_COUNT && uiTickTime - uiPERIOD_COUNT > 2)
    {
        std::cout << "WARNING: Tick time of " << uiTickTime << "ms " <<
                  "exceeds period of " << uiPERIOD_COUNT << "ms " <<
                  "(" << TICKS_PER_SECOND << " tps) significantly!" << std::endl;
    }
}

void cGameLoop::Tick()
{
    // Try locking the mutex if it's available, otherwise print a warning message
    if (poTasksMutex.try_lock())
    {
        // Run all the tick tasks
        for (iTickTask* pTask : papTasks)
        {
            pTask->Tick();
        }
        poTasksMutex.unlock();
    }
    else
    {
        std::cout << "WARNING: Skipped a tick because tasks are locked" << std::endl;
    }
}
