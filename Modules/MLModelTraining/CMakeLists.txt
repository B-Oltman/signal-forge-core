add_library(MLModelTraining STATIC MLModelTraining.cpp)

target_include_directories(MLModelTraining PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
