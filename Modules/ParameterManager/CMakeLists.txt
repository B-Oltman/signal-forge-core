# Add source files for the ParameterManager module
set(SOURCES
    ParameterManager.cpp    
    ParameterDataAccess.cpp
    ParameterDataAccessServer.cpp
)

set(HEADERS   
    ParameterManager.h
    ParameterDataAccess.h
    ParameterGroup.h
    ParameterDataAccessServer.h
)

# Create a library for the module
add_library(ParameterManager ${SOURCES})

# Specify include directories for the module
target_include_directories(ParameterManager PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Link other required libraries (if any)
target_link_libraries(ParameterManager PUBLIC TradingPlatform MLTuningManager CommonTypes Logger)
