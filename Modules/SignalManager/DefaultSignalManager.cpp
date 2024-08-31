#include "DefaultSignalManager.h"
#include "SignalContainer.h"

std::optional<std::vector<PendingOrder>> DefaultSignalManager::GeneratePendingOrders() {
    // Generate new signals using the signal generator
    std::vector<TradeSignal> newSignals = signalGenerator->GenerateSignals();

    // Add new signals to the unprocessed signals map
    for (const auto& signal : newSignals) {
        signalContainer.AddSignal(signal);
    }

    // If running in synchronous mode, process levels and generate signals immediately
    // Otherwise, this is handled in its own thread and called on a loop.
    if(mode_ == Mode::Synchronous && levelManager_) {
        levelManager_->ProcessLevelsAndGenerateSignals();
    } 

    // Process signals and generate pending orders
    std::optional<std::vector<PendingOrder>> pendingOrders = signalProcessor->ProcessSignals();

    // Clear processed signals
    std::vector<TradeSignal> signalsToClear = signalProcessor->GetSignalsToClear();
    for (const auto& signal : signalsToClear) {
        signalContainer.RemoveSignalById(signal.id);
    }

    return pendingOrders;
}
