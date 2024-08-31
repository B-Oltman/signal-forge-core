#pragma once

#include "CommonTypes.h"
#include "ITradingPlatform.h"
#include <vector>
#include <unordered_map>
#include <memory>
#include <mutex>

class LevelProcessor {
public:
    LevelProcessor(std::shared_ptr<ITradingPlatform> tradingPlatform) : tp(tradingPlatform) {}
    virtual ~LevelProcessor() = default;
    virtual std::vector<TradeSignal> ProcessLevels(
        const std::unordered_map<double, std::vector<BaseLevel>>& levels, 
        double currentPrice) = 0;

    virtual std::vector<BaseLevel> GetLevelsToClear(const std::unordered_map<double, std::vector<BaseLevel>>& levels, double currentPrice) = 0;

protected:
    std::shared_ptr<ITradingPlatform> tp;
};
