#include "BacktestingManager.h"

BacktestingManager::BacktestingManager(std::shared_ptr<ITradingPlatform> tradingPlatform) : tp(tradingPlatform)
{};

void BacktestingManager::PrepareForBacktest(DateTime start, DateTime end)
{
    if (!backtestPrepared)
    {        
        // Initialize parameters or other necessary settings for backtesting
        tp->InitializePlatform(ContextType::Backtesting, start, end);
        ParameterManager::Instance()->InitializeParameters(ContextType::Backtesting);        
        backtestPrepared = true;
        backtestStartDate = start;
        backtestEndDate = end;
    }
}

void BacktestingManager::TrackProgress()
{
    // if (isBacktestingComplete(tp)) { TOOD: Implement
    //     SaveResults();
    // }
    // previousIndex = sc.Index;
}

void BacktestingManager::SaveResults()
{
    auto stats = GetBacktestingStats();
    

    TradeSession results;    
    results.tradeStatistics = stats;
    results.startDate = backtestStartDate;
    results.endDate = backtestEndDate;
    
    ParameterManager::Instance()->SendSession(results);    
}

TradeStatistics BacktestingManager::GetBacktestingStats()
{
    return tp->GetAccountWideTradeStatistics();
}
