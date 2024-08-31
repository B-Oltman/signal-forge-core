# Add source files for the FileIO module
set(SOURCES
    FileIO.cpp
    FileIO.h
    FileIOMessaging.cpp
    FileIOMessaging.h
)

# Create a library for the module
add_library(FileIO ${SOURCES})

# Specify include directories for the module
target_include_directories(FileIO PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(FileIO PUBLIC Logger CommonTypes Threading Networking) 
