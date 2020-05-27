#pragma once

#ifndef NDEBUG

#include <chrono>
#include <iostream>

using namespace std::chrono;

#define TIME_SINCE_START (duration_cast<milliseconds>(steady_clock::now() - tEngineStartTime).count())
#define CURRENT_CLASS_NAME (&typeid(*this).name()[3])

#ifdef ENGINE_ENABLE_LOG
#define ENGINE_LOG(message) (std::cout << "[" << TIME_SINCE_START << "] [" << GetCurrentFileName(__FILE__) << "]: " << message << std::endl) //-V1003
#else
#define ENGINE_LOG(message) (void())
#endif

#define ENGINE_WARN(message) (ENGINE_LOG("[WARNING] " << message)) //-V1003

steady_clock::time_point tEngineStartTime = steady_clock::now();

std::string GetCurrentFileName(std::string sPath)
{
    uint uiIndex;
    for (uiIndex = (uint) sPath.size(); uiIndex > 0; uiIndex--)
    {
        if (sPath[uiIndex - 1] == '\\' || sPath[uiIndex - 1] == '/') break;
    }
    std::string sResult = sPath.substr(uiIndex);
    return sResult.substr(0, sResult.length() - 4);
}

#else // NDEBUG
#define ENGINE_WARN(message) (void())
#define ENGINE_LOG(message) (void())
#endif
