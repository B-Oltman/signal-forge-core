#pragma once

#include "CommonTypes.h"
#include "ITradingPlatform.h"
#include <vector>
#include <memory>

class LevelGenerator {
public:
    LevelGenerator(std::shared_ptr<ITradingPlatform> tradingPlatform) : tp(tradingPlatform) {}
    virtual ~LevelGenerator() = default;
    virtual std::vector<BaseLevel> GenerateLevels(double currentPrice) = 0;
protected:     
    std::shared_ptr<ITradingPlatform> tp;
};
