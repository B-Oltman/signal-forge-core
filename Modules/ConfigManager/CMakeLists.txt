# Add source files for the ConfigManager module
set(SOURCES
    ConfigManager.cpp
    ConfigManager.h
)

# Create a library for the module
add_library(ConfigManager ${SOURCES})

# Specify include directories for the module
target_include_directories(ConfigManager PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Link other required libraries (if any)
target_link_libraries(ConfigManager PUBLIC FileIO CommonTypes Logger)
