# Add source files for the Logger module
set(SOURCES
    Logger.cpp
    Logger.h
)

# Create a library for the module
add_library(Logger ${SOURCES})

# Specify include directories for the module
target_include_directories(Logger PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(Logger PUBLIC CommonTypes Threading)
