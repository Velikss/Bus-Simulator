#pragma once

#include <chrono>
#include <iostream>

using namespace std::chrono;

#define TIME_SINCE_START (duration_cast<milliseconds>(steady_clock::now() - tEngineStartTime).count())
#define CURRENT_CLASS_NAME (&typeid(*this).name()[3])

#ifdef ENGINE_ENABLE_LOG
#define ENGINE_LOG(message) (std::cout << "[" << TIME_SINCE_START << "] " << message << std::endl) //-V1003
#else
#define ENGINE_LOG(message) (void())
#endif

#define ENGINE_WARN(message) (std::cout << "[" << TIME_SINCE_START << "] [WARNING] " << message << std::endl) //-V1003

steady_clock::time_point tEngineStartTime = steady_clock::now();
