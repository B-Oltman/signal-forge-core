# Minimum CMake version required
cmake_minimum_required(VERSION 3.10)

# Set the project name
project(quant-core-tests)

# Ensure all libraries use the same runtime
if(MSVC)
  option(gtest_force_shared_crt "Use shared (DLL) run-time lib even when Google Test is built as static lib." ON)
endif()

# Enable testing
enable_testing()

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/Modules/CommonTypes)
include_directories(${CMAKE_SOURCE_DIR}/Modules/OrderExecutor)
include_directories(${CMAKE_SOURCE_DIR}/Modules/RiskManager)
include_directories(${CMAKE_SOURCE_DIR}/Modules/SignalGenerator)
include_directories(${CMAKE_SOURCE_DIR}/Modules/TradeSystem)
include_directories(${CMAKE_SOURCE_DIR}/Modules/Logger)
include_directories(${CMAKE_SOURCE_DIR}/Modules/FileIO)
include_directories(${CMAKE_SOURCE_DIR}/Modules/BacktestingManager)
include_directories(${CMAKE_SOURCE_DIR}/Modules/OrderManager)
include_directories(${CMAKE_SOURCE_DIR}/Modules/ParameterManager)
include_directories(${CMAKE_SOURCE_DIR}/Modules/ConfigManager)
include_directories(${CMAKE_SOURCE_DIR}/Modules/HttpClient)
include_directories(${CMAKE_SOURCE_DIR}/Modules/Timing)
include_directories(${CMAKE_SOURCE_DIR}/Modules/TradingPlatform)
include_directories(${CMAKE_SOURCE_DIR}/external/googletest/googletest/include)
include_directories(${CMAKE_SOURCE_DIR}/external/googletest/googlemock/include)

# Add the test source files
add_executable(runTests  
    BacktestingManager/BacktestingManagerTest.cpp
    CommonTypes/CommonTypesTest.cpp
    ConfigManager/ConfigManagerTest.cpp
    FileIO/FileIOTest.cpp
    HttpClient/HttpClientTest.cpp
    Logger/LoggerTest.cpp    
    OrderExecutor/OrderExecutorTest.cpp
    OrderManager/OrderManagerTest.cpp
    ParameterManager/ParameterManagerTest.cpp    
    RiskManager/RiskManagerTest.cpp
    SignalGenerator/SignalGeneratorTest.cpp
    Timing/TimingTest.cpp    
    TradingPlatform/TradingPlatformTest.cpp
    main.cpp  # Your custom main for logging
)

# Ensure all libraries use the same runtime
if(MSVC)
  set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

# Link the test executable against the necessary libraries
target_link_libraries(runTests
    gtest
    gtest_main
    gmock
    CommonTypes
    OrderExecutor
    RiskManager
    SignalGenerator
    TradeSystem
    Logger
    FileIO
    BacktestingManager
    OrderManager
    ParameterManager
    ConfigManager          
    HttpClient    
    Timing
    TradingPlatform
)

# Include directories for the test executable
target_include_directories(runTests PRIVATE
    ${CMAKE_SOURCE_DIR}/Modules/CommonTypes
    ${CMAKE_SOURCE_DIR}/Modules/OrderExecutor
    ${CMAKE_SOURCE_DIR}/Modules/RiskManager
    ${CMAKE_SOURCE_DIR}/Modules/SignalGenerator
    ${CMAKE_SOURCE_DIR}/Modules/TradeSystem
    ${CMAKE_SOURCE_DIR}/Modules/Logger
    ${CMAKE_SOURCE_DIR}/Modules/FileIO
    ${CMAKE_SOURCE_DIR}/Modules/BacktestingManager
    ${CMAKE_SOURCE_DIR}/Modules/OrderManager
    ${CMAKE_SOURCE_DIR}/Modules/ParameterManager
    ${CMAKE_SOURCE_DIR}/Modules/ConfigManager      
    ${CMAKE_SOURCE_DIR}/Modules/HttpClient        
    ${CMAKE_SOURCE_DIR}/Modules/Timing    
    ${CMAKE_SOURCE_DIR}/external/googletest/googletest/include
    ${CMAKE_SOURCE_DIR}/external/googletest/googlemock/include
)

# Add a test
add_test(NAME runTests COMMAND runTests)
