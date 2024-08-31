#pragma once

#include "LevelProcessor.h"

class DefaultLevelProcessor : public LevelProcessor {
public:
    DefaultLevelProcessor(std::shared_ptr<ITradingPlatform> tradingPlatform) : LevelProcessor(tradingPlatform) {}
    std::vector<TradeSignal> ProcessLevels(const std::unordered_map<double, std::vector<BaseLevel>>& levels, double currentPrice) override;
    std::vector<BaseLevel> GetLevelsToClear(const std::unordered_map<double, std::vector<BaseLevel>>& levels, double currentPrice) override;
};
