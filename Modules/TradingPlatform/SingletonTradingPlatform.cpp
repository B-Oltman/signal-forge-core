#include "SingletonTradingPlatform.h"

// Define the static member variable
std::shared_ptr<ITradingPlatform> SingletonTradingPlatform::instance = nullptr;
