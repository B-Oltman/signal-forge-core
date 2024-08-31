#pragma once
#include <chrono>

class HighPrecisionTimer {
public:
    void Start();
    void Stop();
    double GetElapsedTime() const;

private:
    std::chrono::high_resolution_clock::time_point startTime;
    std::chrono::high_resolution_clock::time_point endTime;
};
