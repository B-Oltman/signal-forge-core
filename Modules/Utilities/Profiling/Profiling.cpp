#include "Profiling.h"

void Profiling::RunBenchmarks(int argc, char** argv) {
    ::benchmark::Initialize(&argc, argv);
    ::benchmark::RunSpecifiedBenchmarks();
}
