#pragma once

#include "LevelGenerator.h"
#include "ITradingPlatform.h"

class DefaultLevelGenerator : public LevelGenerator {
public:
    DefaultLevelGenerator(std::shared_ptr<ITradingPlatform> tradingPlatform) : LevelGenerator(tradingPlatform) {}
    std::vector<BaseLevel> GenerateLevels(double currentPrice) override;
};
