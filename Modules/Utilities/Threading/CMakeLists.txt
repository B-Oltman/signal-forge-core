# Add source files for the FileIO module
set(SOURCES
ThreadPool.cpp
ThreadPool.h
)

# Create a library for the module
add_library(Threading ${SOURCES})

# Specify include directories for the module
target_include_directories(Threading PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(Threading PUBLIC Logger CommonTypes) 
