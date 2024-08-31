# Add source files for the SignalManager module
set(SOURCES    
    DefaultSignalManager.cpp    
    LevelManager/DefaultLevelGenerator.cpp
    LevelManager/DefaultLevelProcessor.cpp
    LevelManager/LevelManager.cpp
    LevelManager/LevelUtilities.cpp
)

# Add header files for the SignalManager module
set(HEADERS    
    SignalManager.h    
    SignalGenerator.h    
    SignalProcessor.h        
    DefaultSignalManager.h
    DefaultSignalGenerator.h
    DefaultSignalProcessor.h           
    SignalContainer.h
    LevelManager/LevelGenerator.h
    LevelManager/DefaultLevelGenerator.h
    LevelManager/DefaultLevelProcessor.h
    LevelManager/LevelManager.h
    LevelManager/LevelUtilities.h
)

# Create a library for the module
add_library(SignalManager ${SOURCES} ${HEADERS})

# Specify include directories for the module
target_include_directories(SignalManager PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/LevelManager
)

# Link other required libraries (if any)
target_link_libraries(SignalManager PUBLIC ParameterManager Logger CommonTypes TradingPlatform)
