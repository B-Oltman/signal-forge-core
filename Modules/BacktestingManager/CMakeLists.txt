# Add source files for the BacktestingManager module
set(SOURCES
    BacktestingManager.cpp
    BacktestingManager.h
)

# Create a library for the module
add_library(BacktestingManager ${SOURCES})

# Specify include directories for the module
target_include_directories(BacktestingManager PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Link other required libraries (if any)
target_link_libraries(BacktestingManager PUBLIC FileIO ParameterManager CommonTypes)
