# Add source files for the FileIO module
set(SOURCES
ThreadSafeQueue.cpp
ThreadSafeQueue.h
)

# Create a library for the module
add_library(Queue ${SOURCES})

# Specify include directories for the module
target_include_directories(Queue PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
