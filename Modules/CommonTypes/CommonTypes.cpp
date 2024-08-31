#include "CommonTypes.h"
#include "ParameterManager.h"

// Define the function to get the trade system name
std::string GetTradeSystemName() {
    return ParameterManager::Instance()->GetSystemName();
}

// Initialize the static members (if required)
std::atomic<int> UniqueIDGenerator::nextId{0}; // Initialize the static member variable