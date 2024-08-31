#ifndef SIGNAL_MANAGER_H
#define SIGNAL_MANAGER_H

#include "SignalGenerator.h"
#include "SignalProcessor.h"
#include "CommonTypes.h"
#include "ITradingPlatform.h"
#include "LevelManager/LevelManager.h"
#include "SignalContainer.h"
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <vector>
#include <optional>
#include <memory>
#include <mutex>

class SignalManager {
public:
    SignalManager(std::shared_ptr<SignalGenerator> generator, std::shared_ptr<SignalProcessor> processor, std::shared_ptr<ITradingPlatform> tradingPlatform, std::shared_ptr<LevelManager> levelManager = nullptr)
        : signalGenerator(generator), signalProcessor(processor), tp(tradingPlatform), levelManager_(levelManager), signalContainer(SignalContainer::Instance()) {
        
        mode_ = Mode::Synchronous;

        if (levelManager) {
            levelManager_->SubscribeToMessages([this](const std::string& message) {
                TradeSignal signal = ParseSignal(message);
                signalContainer.AddSignal(signal);
            });
        }
    }

    virtual std::optional<std::vector<PendingOrder>> GeneratePendingOrders() = 0;

    // Clear processed signals
    void ClearProcessedSignals() {
        std::vector<TradeSignal> signalsToClear = signalProcessor->GetSignalsToClear();
        for (const auto& signal : signalsToClear) {
            signalContainer.RemoveSignalById(signal.id);
        }
    }

protected:
    std::shared_ptr<ITradingPlatform> tp;
    std::shared_ptr<LevelManager> levelManager_;
    SignalContainer& signalContainer;
    std::shared_ptr<SignalGenerator> signalGenerator;
    std::shared_ptr<SignalProcessor> signalProcessor;
    Mode mode_;
    mutable std::mutex mtx_;

    TradeSignal ParseSignal(const std::string& message) {        
        TradeSignal signal = TradeSignal::FromJsonString(message);        
        return signal;
    }
};

#endif // SIGNAL_MANAGER_H
