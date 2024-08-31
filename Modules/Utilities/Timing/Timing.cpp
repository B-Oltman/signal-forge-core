#include "Timing.h"

void HighPrecisionTimer::Start() {
    startTime = std::chrono::high_resolution_clock::now();
}

void HighPrecisionTimer::Stop() {
    endTime = std::chrono::high_resolution_clock::now();
}

double HighPrecisionTimer::GetElapsedTime() const {
    std::chrono::duration<double> elapsedTime = endTime - startTime;
    return elapsedTime.count();
}
