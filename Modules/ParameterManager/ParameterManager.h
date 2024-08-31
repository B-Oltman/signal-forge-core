#ifndef PARAMETER_MANAGER_H
#define PARAMETER_MANAGER_H

#include "CommonTypes.h"
#include "ITradingPlatform.h"
#include "MLTuningManager.h"
#include "ParameterDataAccess.h"
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include <shared_mutex>
#include <atomic>
#include <chrono>
#include <optional>

class ParameterDataAccess;

class ParameterManager {
public:
    static std::shared_ptr<ParameterManager> Instance();

    ~ParameterManager();
    ParameterManager(const ParameterManager&) = delete;
    void operator=(const ParameterManager&) = delete;

    void InitializeParameterManager(const std::string& systemName);
    void InitializeMLTuningManager(std::shared_ptr<ITuningDataProcessor> tuner);
    void InitializeParameters(ContextType contextType, const std::optional<std::string>& specificId = std::nullopt);
    ParameterValue GetParameter(const std::string& key) const;
    std::vector<ParameterValue> GetAllParameterValues() const;    
    void UpdateParameterGroup(const TradeSystemParameterGroup& newGroup);
    void UpdateTradingSystem(const TradingSystem& tradingSystem);
    void SendSession(const TradeSession& sessionResult);
    std::string GetSystemName() const;
    const TradeSystemParameterGroup& GetParameterGroup() const;
    const TradingSystem& GetTradingSystem() const;
    const bool GetIsTradeSystemStale() const;
    const void ResetTradeSystemStaleStatus();

private:
    ParameterManager();        

    mutable std::shared_mutex mtx;    
    std::shared_ptr<ParameterDataAccess> parameterDataAccess;
    std::shared_ptr<MLTuningManager> _MLTuningManager;
    TradeSystemParameterGroup parameterGroup;
    TradingSystem tradingSystem;    
    std::string systemName_;
    bool tradeSystemStale;    

    static std::shared_ptr<ParameterManager> instance_;
};

#endif // PARAMETER_MANAGER_H
