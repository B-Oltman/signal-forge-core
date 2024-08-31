#ifndef DEFAULT_SIGNAL_MANAGER_H
#define DEFAULT_SIGNAL_MANAGER_H

#include "SignalManager.h"
#include "SignalGenerator.h"
#include "SignalProcessor.h"
#include "LevelManager/LevelManager.h"
#include <optional>
#include <vector>
#include <unordered_map>
#include <string>

class DefaultSignalManager : public SignalManager {
public:
    DefaultSignalManager(std::shared_ptr<SignalGenerator> generator, std::shared_ptr<SignalProcessor> processor, std::shared_ptr<ITradingPlatform> tradingPlatform, std::shared_ptr<LevelManager> levelManager = nullptr)
        : SignalManager(generator, processor, tradingPlatform, levelManager) {}

    std::optional<std::vector<PendingOrder>> GeneratePendingOrders() override;
};

#endif // DEFAULT_SIGNAL_MANAGER_H
