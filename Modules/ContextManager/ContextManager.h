#pragma once

#include "sierrachart.h"
#include "ParameterManager.h"
#include "FileIO.h"
#include "Logger.h"
#include "Timing.h"
#include <memory>
#include <string>
#include "BacktestingManager.h"
#include "ITradingPlatform.h"
#include "CommonTypes.h"

class ContextManager {
public:
    ContextManager(std::shared_ptr<ITradingPlatform> tradingPlatform, std::string systemName, std::optional<std::shared_ptr<ITuningDataProcessor>> parameterTuner = std::nullopt);
    void Initialize();


    bool IsTradingDay(const DateTime& currentTime);
    bool ManageSession(int throttleIntervalMilliseconds = 0);

private:
    void InitializeForBacktestingContext();
    void InitializeForPaperTradingContext();
    void InitializeForLiveTradingContext();
    bool ShouldProceed(int throttleIntervalMilliseconds = 0);

    std::shared_ptr<TradeSession> CreateNewSession(const DateTime& startTime);
    void UpdateSessionStatistics(const std::shared_ptr<TradeSession>& session);
    void SaveCurrentSession(bool endSession = false);

    std::shared_ptr<ITradingPlatform> tp;
    std::shared_ptr<BacktestingManager> backtestingManager;    
    ContextType contextType;
    std::shared_ptr<FileIO> fileIO_;        

    // Throttling-related members
    double previousTime = 0.0;
    HighPrecisionTimer timer;

    DateTime lastSnapshotTime_;
    int snapshotIntervalMinutes_;    

    // Session management
    std::shared_ptr<TradeSession> currentSession;  // Track the current session
};

