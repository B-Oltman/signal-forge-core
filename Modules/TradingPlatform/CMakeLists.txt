# Add source files for the TradingPlatform module
set(SOURCES
    SingletonTradingPlatform.cpp
)

set(HEADER
    ITradingPlatform.h    
    SierraChartPlatform.h
    SingletonTradingPlatform.h    
    SierraChartHelpers.h)

# Create a library for the module
add_library(TradingPlatform ${SOURCES})

# Set the language for the TradingPlatform target
set_target_properties(TradingPlatform PROPERTIES LINKER_LANGUAGE CXX)

# Include directories for this module
target_include_directories(TradingPlatform PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Link libraries for this module
target_link_libraries(TradingPlatform PUBLIC CommonTypes)
