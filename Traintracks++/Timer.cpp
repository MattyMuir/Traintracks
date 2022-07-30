#include "Timer.h"

#include <iostream>
#include <sstream>
using namespace std::chrono;

Timer::Timer()
    : startTimePoint(high_resolution_clock::now()) {}

void Timer::StopLog(std::string_view timerName)
{
    duration x = high_resolution_clock::now() - startTimePoint;
    uint64_t microSeconds = duration_cast<microseconds>(x).count();

    std::stringstream ss;
    if (microSeconds < 1000)
        ss << timerName << " took: " << microSeconds << "us\n";
    else
        ss << timerName << " took: " << microSeconds * 0.001 << "ms\n";

    std::cout << ss.str() << std::flush;
}