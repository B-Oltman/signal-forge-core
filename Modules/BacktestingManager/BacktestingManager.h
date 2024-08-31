#pragma once

#include <memory>
#include "ParameterManager.h"
#include <nlohmann/json.hpp>
#include "ITradingPlatform.h"
#include "SingletonTradingPlatform.h"
#include "CommonTypes.h"

class BacktestingManager {
public:
    BacktestingManager(std::shared_ptr<ITradingPlatform> tradingPlatform);        
    void PrepareForBacktest(DateTime start, DateTime end);
    void TrackProgress();
    void SaveResults();
    TradeStatistics GetBacktestingStats();

private:
    std::shared_ptr<ITradingPlatform> tp;    
    int previousIndex = -1;
    DateTime backtestStartDate;
    DateTime backtestEndDate;
    bool backtestPrepared = false;
    

    bool isBacktestingComplete() {
        // Example implementation, modify as needed
        // TODO: Implement this, make it more robust.
        //return tp.Index == tp.ArraySize - 1;
    }
};
