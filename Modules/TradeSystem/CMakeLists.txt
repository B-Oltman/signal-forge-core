# TradeSystem CMakeLists.txt

# Add source files for the TradeSystem module
set(SOURCES
    TradeSystem.cpp
    TradeSystem.h    
    TradeSystemBuilder.h
    verify_template.cpp    
)

# Create a library for the module
add_library(TradeSystem ${SOURCES})

# Specify include directories for the module
target_include_directories(TradeSystem PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Link dependencies
target_link_libraries(TradeSystem PUBLIC SignalManager RiskManager OrderManager ContextManager CommonTypes Filters)
