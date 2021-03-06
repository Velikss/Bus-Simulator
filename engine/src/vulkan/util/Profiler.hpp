#pragma once

#include <pch.hpp>
#include <chrono>
#include <vulkan/util/EngineLog.hpp>

#ifndef NDEBUG
#define START_TIMING(name) (cProfiler::poInstance.StartTiming(name))
#define STOP_TIMING(name) (cProfiler::poInstance.StopTiming(name))
#else
#define START_TIMING(name) (void())
#define STOP_TIMING(name) (void())
#endif

using namespace std::chrono;

typedef time_point<steady_clock, nanoseconds> tTimePoint;
typedef steady_clock tClock;

class cProfiler
{
public:
    static cProfiler poInstance;

private:
    tTimePoint ptStartTime = tClock::now();
    uint puiFrameCount = 0;

    uint puiFramesPerSecond = 0;

    std::map<string, steady_clock::time_point> pmtTimings;

public:
    void Update();

    uint GetFramesPerSecond();
    float GetFrameTime();

#ifndef NDEBUG
    void StartTiming(const string& sName);
    uint StopTiming(const string& sName);
#endif
};

cProfiler cProfiler::poInstance;

void cProfiler::Update()
{
    puiFrameCount++;

    tTimePoint tNow = tClock::now();
    float fDelta = duration<float, std::chrono::seconds::period>(tNow - ptStartTime).count();

    if (fDelta >= 1)
    {
        puiFramesPerSecond = puiFrameCount;

#ifdef ENABLE_FPS_COUNT
        ENGINE_LOG(GetFramesPerSecond() << " fps (avg " << GetFrameTime() << "ms)");
#endif

        ptStartTime = tNow;
        puiFrameCount = 0;
    }
}

uint cProfiler::GetFramesPerSecond()
{
    return puiFramesPerSecond;
}

float cProfiler::GetFrameTime()
{
    return 1000.0f / float(puiFramesPerSecond);
}

#ifndef NDEBUG
void cProfiler::StartTiming(const string& sName)
{
    pmtTimings[sName] = steady_clock::now();
}

uint cProfiler::StopTiming(const string& sName)
{
    uint duration = (uint)(duration_cast<milliseconds>(steady_clock::now() - pmtTimings[sName]).count());
    pmtTimings.erase(sName);
    return duration;
}
#endif
