# Add source files for the Timing module
set(SOURCES
    Timing.cpp
    Timing.h
)

# Create a library for the module
add_library(Timing ${SOURCES})

# Specify include directories for the module
target_include_directories(Timing PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
