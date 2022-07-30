#pragma once
#include <chrono>

#define TIMING 1

#if TIMING
#define TIMER(x) Timer x
#define STOP_LOG(x) x.StopLog(#x)
#else
#define TIMER(x)
#define STOP_LOG(x)
#endif

class Timer
{
public:
    Timer();
    void StopLog(std::string_view timerName = "");

private:
    std::chrono::time_point<std::chrono::high_resolution_clock> startTimePoint;
};