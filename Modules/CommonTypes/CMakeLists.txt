set(SOURCES
    CommonTypes.cpp
    CommonTypes.h
)

# Create a library for the module
add_library(CommonTypes ${SOURCES})

# Specify include directories for the module
target_include_directories(CommonTypes PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(CommonTypes PUBLIC ParameterManager)
