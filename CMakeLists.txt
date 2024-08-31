# Set the minimum required version of CMake
cmake_minimum_required(VERSION 3.21)

# Define the project name and version
project(signal-forge-core VERSION 1.0)

# Set the C++ standard to use (C++17)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)


# Add preprocessor definitions globally
add_definitions(-DWIN32_LEAN_AND_MEAN)
add_definitions(-DBOOST_ASIO_NO_WIN32_LEAN_AND_MEAN)
add_definitions(-D_WIN32_WINNT=0x0A00)  # Windows 11

# Ensure all libraries use the same runtime
if(MSVC)
  option(gtest_force_shared_crt "Use shared (DLL) run-time lib even when Google Test is built as static lib." ON)
endif()

# Include directories for Sierra Chart source code
include_directories("C:/SierraChart/ACS_Source")

# Include directories for external dependencies
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/external)

# Set build type to Debug to include debug symbols
set(CMAKE_BUILD_TYPE Debug)

# Set compiler and linker flags for debug builds
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Zi")
set(CMAKE_SHARED_LINKER_FLAGS_DEBUG "${CMAKE_SHARED_LINKER_FLAGS_DEBUG} /DEBUG")

# Set policies for Boost
cmake_policy(SET CMP0167 NEW)
cmake_policy(SET CMP0144 NEW)

# Specify the path to the Boost installation
set(BOOST_ROOT "C:/Users/Baruc/source/repos/boost_1_85_0")
set(Boost_DIR "${BOOST_ROOT}/lib64-msvc-14.3/cmake/Boost-1.85.0")

# Specify the required version of Boost
set(BOOST_MIN_VERSION "1.85.0")

# Find Boost libraries
find_package(Boost ${BOOST_MIN_VERSION} REQUIRED COMPONENTS system filesystem)

if (Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS})
  add_definitions(-DBOOST_ALL_NO_LIB)
else()
  message(FATAL_ERROR "Could not find Boost libraries. Please ensure Boost is installed and BOOST_ROOT is set correctly.")
endif()

# Specify the path to Google Benchmark installation
set(BENCHMARK_ROOT "C:/Program Files (x86)/benchmark")
set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} "${BENCHMARK_ROOT}")

# Find Google Benchmark
find_package(benchmark REQUIRED)

if (benchmark_FOUND)
  include_directories(${BENCHMARK_ROOT}/include)
  link_directories(${BENCHMARK_ROOT}/lib)
  link_libraries(benchmark)
else()
  message(FATAL_ERROR "Could not find Google Benchmark. Please ensure it is installed and BENCHMARK_ROOT is set correctly.")
endif()

# Include Google Test (which includes Google Mock)
add_subdirectory(external/googletest)

# Add subdirectories for each module
add_subdirectory(Modules/CommonTypes)
add_subdirectory(Modules/RiskManager)
add_subdirectory(Modules/SignalManager)
add_subdirectory(Modules/TradeSystem)
add_subdirectory(Modules/BacktestingManager)
add_subdirectory(Modules/OrderManager)
add_subdirectory(Modules/Filters)
add_subdirectory(Modules/ParameterManager)
add_subdirectory(Modules/MLTuningManager)
add_subdirectory(Modules/ConfigManager)
add_subdirectory(Modules/ContextManager)
add_subdirectory(Modules/TradingPlatform)
add_subdirectory(Modules/Utilities/Calculations)
add_subdirectory(Modules/Utilities/Timing)
add_subdirectory(Modules/Utilities/Threading)
add_subdirectory(Modules/Utilities/Queue)
add_subdirectory(Modules/Utilities/Profiling)
add_subdirectory(Modules/Utilities/Networking)
add_subdirectory(Modules/Utilities/FileIO)
add_subdirectory(Modules/Utilities/Logger)

# Collect all targets into a single interface library
add_library(signal-forge-core INTERFACE) ## TODO Link this as a static library instead of interface. Need to make sure every module uses a cpp file. Then we should
# be able to avoid adding references such as benchmark directly in our trade system implementations

# Link libraries to the interface library
target_link_libraries(signal-forge-core INTERFACE
  CommonTypes
  Calculations  
  RiskManager
  SignalManager
  TradeSystem
  Logger
  FileIO
  Queue
  Threading
  BacktestingManager
  OrderManager
  Filters
  ParameterManager
  MLTuningManager
  ConfigManager
  ContextManager
  Networking
  Profiling
  Timing
  ${Boost_LIBRARIES}
  benchmark
  TradingPlatform)

# Include directories for the interface library
target_include_directories(signal-forge-core INTERFACE
  ${CMAKE_CURRENT_SOURCE_DIR}/Modules/CommonTypes
  ${CMAKE_CURRENT_SOURCE_DIR}/Modules/RiskManager
  ${CMAKE_CURRENT_SOURCE_DIR}/Modules/SignalManager
  ${CMAKE_CURRENT_SOURCE_DIR}/Modules/TradeSystem
  ${CMAKE_CURRENT_SOURCE_DIR}/Modules/BacktestingManager
  ${CMAKE_CURRENT_SOURCE_DIR}/Modules/OrderManager
  ${CMAKE_CURRENT_SOURCE_DIR}/Modules/Filters
  ${CMAKE_CURRENT_SOURCE_DIR}/Modules/ParameterManager
  ${CMAKE_CURRENT_SOURCE_DIR}/Modules/MLTuningManager
  ${CMAKE_CURRENT_SOURCE_DIR}/Modules/ConfigManager
  ${CMAKE_CURRENT_SOURCE_DIR}/Modules/ContextManager  
  ${CMAKE_CURRENT_SOURCE_DIR}/Modules/Utilities/Calculations  
  ${CMAKE_CURRENT_SOURCE_DIR}/Modules/Utilities/Timing
  ${CMAKE_CURRENT_SOURCE_DIR}/Modules/Utilities/Threading
  ${CMAKE_CURRENT_SOURCE_DIR}/Modules/Utilities/Queue
  ${CMAKE_CURRENT_SOURCE_DIR}/Modules/Utilities/Profiling
  ${CMAKE_CURRENT_SOURCE_DIR}/Modules/Utilities/Networking
  ${CMAKE_CURRENT_SOURCE_DIR}/Modules/Utilities/FileIO
  ${CMAKE_CURRENT_SOURCE_DIR}/Modules/Utilities/Logger
  ${Boost_INCLUDE_DIRS}
  ${BENCHMARK_ROOT}/include
  ${CMAKE_CURRENT_SOURCE_DIR}/Modules/TradingPlatform
  "C:/SierraChart/ACS_Source"
)

# Output debug files (PDBs)
set(CMAKE_PDB_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/Debug)
