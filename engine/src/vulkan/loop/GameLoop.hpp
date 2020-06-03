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
    std::vector<iTickTask*> papStagingTasks;
    bool pbTasksInvalid = false;

    // If false, the loop will terminate
    bool pbRunning = true;
    bool pbPaused = false;

public:
    // Destroys all the tasks inside this loop
    ~cGameLoop();

    // Starts this GameLoop
    void operator()();

    // Add a task to run every tick
    void AddTask(iTickTask* pTask);
    void RemoveTask(iTickTask* pTask);

    // Stop this GameLoop
    void Stop();

    void SetPaused(bool bPaused);

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
}

void cGameLoop::operator()()
{
    pbRunning = true;
    MainLoop();
}

void cGameLoop::AddTask(iTickTask* pTask)
{
    papStagingTasks.push_back(pTask);
    pbTasksInvalid = true;

    ENGINE_LOG("Added new task to game loop");
}

void cGameLoop::RemoveTask(iTickTask* pTask)
{
    for (auto it = papStagingTasks.begin(); it < papStagingTasks.end(); it++)
    {
        if (*it == pTask)
        {
            papStagingTasks.erase(it);
            pbTasksInvalid = true;
            break;
        }
    }
}

void cGameLoop::Stop()
{
    pbRunning = false;
}

void cGameLoop::SetPaused(bool bPaused)
{
    pbPaused = bPaused;
}

void cGameLoop::MainLoop()
{
    // tNext is the time at which the next tick should start
    time_point tNext = steady_clock::now();

    // tPrev is the time when the last tick started
    time_point tPrev = tNext - tPERIOD;

    ENGINE_LOG("Game loop started");

    while (pbRunning)
    {
        // Run tick time checks
        time_point tNow = steady_clock::now();
        CheckTickTime(tPrev, tNow);
        tPrev = tNow;

        if (!pbPaused)
        {
            // Run all the tick tasks
            Tick();
        }

        if (pbTasksInvalid)
        {
            papTasks.clear();
            for (iTickTask* pTask : papStagingTasks)
            {
                papTasks.push_back(pTask);
            }
            pbTasksInvalid = false;
        }

        // The next tick should start PERIOD milliseconds after the last one
        tNext += tPERIOD;
        std::this_thread::sleep_until(tNext);
    }

    ENGINE_LOG("Game loop stopped");
}

void cGameLoop::CheckTickTime(time_point<steady_clock> tPrev, time_point<steady_clock> tNow)
{
    const uint uiPERIOD_COUNT = tPERIOD.count();

    // Calculate the time this tick took
    uint uiTickTime = (uint) round<milliseconds>(tNow - tPrev).count();

    // If the time exceeds a threshold, print a warning
    if (uiTickTime > uiPERIOD_COUNT && uiTickTime - uiPERIOD_COUNT > 2)
    {
        ENGINE_WARN("Tick time of " << uiTickTime << "ms " << "exceeds period of " << uiPERIOD_COUNT
                                    << "ms " << "(" << TICKS_PER_SECOND << " tps) significantly!");
    }
}

void cGameLoop::Tick()
{
    // Run all the tick tasks
    for (iTickTask* pTask : papTasks)
    {
        pTask->Tick();
    }
}
