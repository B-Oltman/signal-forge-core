# Add source files for the ParameterManager module
set(SOURCES
    MLTuningManager.cpp
    MLTuningManagerServer.cpp
    MLTuningManagerServer.h
    MLTuningManager.h
    ITuningDataProcessor.h
)

# Create a library for the module
add_library(MLTuningManager ${SOURCES})

# Specify include directories for the module
target_include_directories(MLTuningManager PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Link other required libraries (if any)
target_link_libraries(MLTuningManager PUBLIC CommonTypes ParameterManager Queue Logger Threading Networking)
