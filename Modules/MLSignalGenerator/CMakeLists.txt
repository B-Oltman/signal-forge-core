# MLSignalGenerator/CMakeLists.txt
add_library(MLSignalGenerator STATIC MLSignalGenerator.cpp)

target_include_directories(MLSignalGenerator PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(MLSignalGenerator PUBLIC FileIO)
