#include "ParameterManager.h"
#include "Logger.h"
#include <stdexcept>
#include <algorithm>

std::shared_ptr<ParameterManager> ParameterManager::instance_;

std::shared_ptr<ParameterManager> ParameterManager::Instance() {
    if (!instance_) {
        instance_ = std::shared_ptr<ParameterManager>(new ParameterManager());
    }
    return instance_;
}

ParameterManager::ParameterManager() {}

ParameterManager::~ParameterManager() {
    parameterDataAccess.reset();
    _MLTuningManager->~MLTuningManager();
    _MLTuningManager.reset();
    instance_.reset();
}

void ParameterManager::InitializeParameterManager(const std::string& systemName) {
    systemName_ = systemName;
    parameterDataAccess = std::make_shared<ParameterDataAccess>(std::make_shared<Messaging>(), *this);
}

void ParameterManager::InitializeMLTuningManager(std::shared_ptr<ITuningDataProcessor> tuner) {
    _MLTuningManager = std::make_shared<MLTuningManager>(tuner, *this, std::make_shared<Messaging>());
}

std::string ParameterManager::GetSystemName() const {
    return systemName_;
}

void ParameterManager::InitializeParameters(ContextType contextType, const std::optional<std::string>& specificId) {
    parameterDataAccess->FetchTradingSystemFromDatabase(systemName_, true);
    parameterDataAccess->FetchParameterGroupFromDatabase(systemName_, specificId, true);
}

void ParameterManager::UpdateParameterGroup(const TradeSystemParameterGroup& newGroup) {
    std::unique_lock<std::shared_mutex> lock(mtx);
    parameterGroup = newGroup;
}

void ParameterManager::UpdateTradingSystem(const TradingSystem& newTradingSystem) {
    std::unique_lock<std::shared_mutex> lock(mtx);
    tradingSystem = newTradingSystem;
    tradeSystemStale = true;
}

void ParameterManager::SendSession(const TradeSession& sessionResult) {
    parameterDataAccess->SendSessionToServer(sessionResult);
}

const bool ParameterManager::GetIsTradeSystemStale() const
{
    std::unique_lock<std::shared_mutex> lock(mtx);
    return tradeSystemStale;
}

const void ParameterManager::ResetTradeSystemStaleStatus()
{
    std::unique_lock<std::shared_mutex> lock(mtx);
    tradeSystemStale = false;
}

ParameterValue ParameterManager::GetParameter(const std::string& key) const {
    std::shared_lock<std::shared_mutex> lock(mtx);
    auto param = parameterGroup.GetParameter(key);
    if (param) {
        return param->value;
    }
    return nullptr;
}

std::vector<ParameterValue> ParameterManager::GetAllParameterValues() const {
    std::shared_lock<std::shared_mutex> lock(mtx);
    std::vector<ParameterValue> values;
    for (const auto& pair : parameterGroup.parameters) {
        values.push_back(pair.second.value);
    }
    return values;
}

const TradeSystemParameterGroup& ParameterManager::GetParameterGroup() const {
    std::shared_lock<std::shared_mutex> lock(mtx);
    return parameterGroup;
}

const TradingSystem& ParameterManager::GetTradingSystem() const {
    std::shared_lock<std::shared_mutex> lock(mtx);
    return tradingSystem;
}
