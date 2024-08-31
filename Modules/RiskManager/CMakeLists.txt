# Add source files for the RiskManager module
set(SOURCES
    DefaultRiskManager.cpp
    RiskProcessors/DefaultActiveOrderRiskProcessor.cpp
    RiskProcessors/DefaultPendingOrderRiskProcessor.cpp
    RiskProcessors/DefaultPositionRiskProcessor.cpp
)

set(HEADERS
    RiskManager.h    
    DefaultRiskManager.h
    RiskProcessors/ActiveOrderRiskProcessor.h
    RiskProcessors/PendingOrderRiskProcessor.h
    RiskProcessors/PositionRiskProcessor.h
    RiskProcessors/DefaultActiveOrderRiskProcessor.h
    RiskProcessors/DefaultPendingOrderRiskProcessor.h
    RiskProcessors/DefaultPositionRiskProcessor.h
)

# Create a library for the module
add_library(RiskManager ${SOURCES})

# Specify include directories for the module
target_include_directories(RiskManager PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/RiskProcessors
)

# Link dependencies
target_link_libraries(RiskManager PUBLIC CommonTypes TradingPlatform)
