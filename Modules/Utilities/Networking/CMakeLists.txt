# HttpClient CMakeLists.txt

# Add source files for the HttpClient module
set(SOURCES
    HttpClient.cpp
    HttpClient.h
    Server.h
    Server.cpp
    Messaging.h
    Messaging.cpp
)

# Create a library for the module
add_library(Networking ${SOURCES})

# Specify include directories for the module
target_include_directories(Networking PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(Networking PUBLIC Logger Threading ParameterManager CommonTypes Queue)
