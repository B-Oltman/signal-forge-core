#include "ContextManager.h"
#include "ParameterManager.h"
#include "Logger.h"
#include <nlohmann/json.hpp>
#include <fstream>

ContextManager::ContextManager(std::shared_ptr<ITradingPlatform> tradingPlatform, std::string systemName, std::optional<std::shared_ptr<ITuningDataProcessor>> parameterTuner)
    : backtestingManager(std::make_shared<BacktestingManager>(tradingPlatform)),
      tp(tradingPlatform)
{
    ParameterManager::Instance()->InitializeParameterManager(systemName);

    if (parameterTuner.has_value()) {
        ParameterManager::Instance()->InitializeMLTuningManager(parameterTuner.value());
    }
}

void ContextManager::Initialize() {
    try {
        auto contextType = tp->GetPlatformContext();        
        if (contextType == ContextType::Backtesting) {
            InitializeForBacktestingContext();
        } else if(contextType == ContextType::LiveTrading) {
            InitializeForLiveTradingContext();
        } else {
            InitializeForPaperTradingContext();
        }

        auto tradingSystem = ParameterManager::Instance()->GetTradingSystem();
        // Retrieve settings from ParameterManager
        auto barSettings = tradingSystem.GetBarTypeAndPeriod();
        std::pair<std::string, std::string> barSettingsValue;
        if(!barSettings.has_value())
        {
            Logger::Log("Not able to get bar period settings.", Logger::LogLevel::LOG_ERROR);            
            barSettingsValue = {"m", "5"}; // Default bar period 5 minute
        } else {
            barSettingsValue = barSettings.value();
        }

        std::string barType = barSettingsValue.first;
        std::string barPeriod = barSettingsValue.second;
        auto updateIntervalType = tradingSystem.GetUpdateIntervalType().value_or(UpdateIntervalType::New_Bar);
        snapshotIntervalMinutes_ = tradingSystem.GetLiveResultsSnapshotInterval().value_or(10);
        lastSnapshotTime_ = DateTime();

        // Set bar type, bar period, and update interval
        tp->SetBarPeriod(barType, barPeriod);
        tp->SetUpdateIntervalType(updateIntervalType);

        // Parameters have been updated, reset the needs update state.
        ParameterManager::Instance()->ResetTradeSystemStaleStatus();

    } catch (const std::exception& e) {
        Logger::Log("Error initializing context: " + std::string(e.what()), Logger::LogLevel::LOG_ERROR);
    }
}

void ContextManager::InitializeForBacktestingContext() {
    try {
        //TODO: Get this from config or somewhere else.
        DateTime endDate = DateTime();
        DateTime startDate = endDate.AddDays(-30);
        backtestingManager->PrepareForBacktest(startDate, endDate);
    } catch (const std::exception& e) {
        Logger::Log("Error initializing backtesting context: " + std::string(e.what()), Logger::LogLevel::LOG_ERROR);
    }
}

void ContextManager::InitializeForLiveTradingContext() {
    try {
        ParameterManager::Instance()->InitializeParameters(ContextType::LiveTrading);        
        tp->InitializePlatform(ContextType::LiveTrading);
    } catch (const std::exception& e) {
        Logger::Log("Error initializing live trading context: " + std::string(e.what()), Logger::LogLevel::LOG_ERROR);
    }
}

void ContextManager::InitializeForPaperTradingContext() {
    try {
        ParameterManager::Instance()->InitializeParameters(ContextType::PaperTrading);        
        tp->InitializePlatform(ContextType::PaperTrading);
    } catch (const std::exception& e) {
        Logger::Log("Error initializing paper trading context: " + std::string(e.what()), Logger::LogLevel::LOG_ERROR);
    }
}

bool ContextManager::ShouldProceed(int throttleIntervalMilliseconds) {
    if(!tp->IsReadyForTradeIteration())
    {
        return false;
    }
    
    timer.Stop();
    double elapsedTime = timer.GetElapsedTime() * 1000.0;

    if (previousTime + elapsedTime < throttleIntervalMilliseconds) {
        previousTime += elapsedTime;
        timer.Start();
        return false;
    }

    previousTime = 0.0;
    timer.Start();
    return true;
}

// This will handle session management and determine if the trade iteration should proceed.
bool ContextManager::ManageSession(int throttleIntervalMilliseconds)
{
    // Save snapshot if the interval has passed    
    DateTime currentTime = tp->GetCurrentPlatformTime();
    if (currentTime - lastSnapshotTime_ >= std::chrono::minutes(snapshotIntervalMinutes_)) {
        UpdateSessionStatistics(false);
        lastSnapshotTime_ = currentTime;
    } 

    if (IsTradingDay(currentTime))
    {
        if (currentSession == nullptr)
        {
            // Start a new session if within the trading window
            currentSession = CreateNewSession(currentTime);
        }
        else
        {
            // Update the session statistics
            UpdateSessionStatistics(currentSession);
        }
    }
    else
    {
        if (currentSession != nullptr)
        {
            // End the session if the trading window is closed
            SaveCurrentSession(true);
            currentSession = nullptr;
        }

        return false;
    }

    return ShouldProceed(throttleIntervalMilliseconds);
}

bool ContextManager::IsTradingDay(const DateTime& currentTime)
{
    auto tradingSystem = ParameterManager::Instance()->GetTradingSystem();

    // Get the current day of the week
    std::string currentDay = currentTime.GetDayOfWeek();

    auto tradingWindow = tradingSystem.GetTradingWindow(currentDay);
    
    if(!tradingWindow.has_value())
    {
        return false;
    }

     if(tradingSystem.IsLoggingEnabled())
     {
         Logger::Log("CurrentTime System Time: " + currentTime.ToString(), Logger::LogLevel::LOG_INFO);
         Logger::Log("Trade Window Start: " + tradingWindow->GetStartTime(&currentTime).ToString(), Logger::LogLevel::LOG_INFO);
         Logger::Log("Trade Window End: " + tradingWindow->GetEndTime(&currentTime).ToString(), Logger::LogLevel::LOG_INFO);
     }    

    auto tradingWindowValue = tradingWindow.value();
    if (currentTime >= tradingWindowValue.GetStartTime(&currentTime) && currentTime <= tradingWindowValue.GetEndTime(&currentTime))
    {
            return true;  // We are within the trading window for today
    }

    return false;  // No trading scheduled for today
}

std::shared_ptr<TradeSession> ContextManager::CreateNewSession(const DateTime& startTime)
{
    auto session = std::make_shared<TradeSession>();
    session->contextType = tp->GetPlatformContext();
    session->startDate = startTime;
    session->parameterGroupId = ParameterManager::Instance()->GetParameterGroup().id;
    session->tradeStatistics = TradeStatistics();    

    ParameterManager::Instance()->SendSession(*session);    
    // Initialize session and potentially save it to a database or log file
    return session;
}

void ContextManager::UpdateSessionStatistics(const std::shared_ptr<TradeSession>& session)
{
    // Collect and update statistics for the current session
    session->tradeStatistics = tp->GetCurrentSessionTradeStatistics();
    // You may also save the session state to a database or log file
}

void ContextManager::SaveCurrentSession(bool endSession)
{
    if (currentSession)
    {
        UpdateSessionStatistics(currentSession);
        ParameterManager::Instance()->SendSession(*currentSession);                

        if(endSession)
        {
            currentSession->endDate = tp->GetCurrentPlatformTime();
            currentSession = nullptr;
        }        
    }
}
