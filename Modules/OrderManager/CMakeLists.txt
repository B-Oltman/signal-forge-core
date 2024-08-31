# OrderManager CMakeLists.txt

# Add source files for the OrderManager module
set(SOURCES
    DefaultOrderManager.cpp    
    DefaultOrderProcessor.h
)

set(HEADERS
    DefaultOrderManager.h
    OrderManager.h
    IOrderProcessor.h    
)

# Create a library for the module
add_library(OrderManager ${SOURCES} ${HEADERS})

# Specify include directories for the module
target_include_directories(OrderManager PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Link dependencies
target_link_libraries(OrderManager PUBLIC CommonTypes TradingPlatform)
