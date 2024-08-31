# Add source files for the Profiling module
set(SOURCES
    Profiling.cpp
    Profiling.h
)

# Create a library for the module
add_library(Profiling ${SOURCES})

# Specify include directories for the module
target_include_directories(Profiling PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
