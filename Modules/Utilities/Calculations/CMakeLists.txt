# Add source files for the Calculations module
set(SOURCES
    Calculations.cpp
    Calculations.h
)

# Create a library for the module
add_library(Calculations ${SOURCES})

# Specify include directories for the module
target_include_directories(Calculations PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ../../CommonTypes)
