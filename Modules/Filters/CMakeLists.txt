# Filters Module

# Define the headers
set(HEADERS
    IPendingOrderFilter.h
)

# Create a library for the module
add_library(Filters INTERFACE)

# Specify include directories for the module
target_include_directories(Filters INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

# Link dependencies
target_link_libraries(Filters INTERFACE CommonTypes TradingPlatform)
