# Add source files for the ContextManager module
set(SOURCES
    ContextManager.cpp
    ContextManager.h    
)

# Create a library for the module
add_library(ContextManager ${SOURCES})

# Specify include directories for the module
target_include_directories(ContextManager PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Link other required libraries (if any)
target_link_libraries(ContextManager PUBLIC ParameterManager Logger BacktestingManager MLTuningManager Timing)
