#ifndef SIGNAL_PROCESSOR_H
#define SIGNAL_PROCESSOR_H

#include "CommonTypes.h"
#include "ITradingPlatform.h"
#include "SignalContainer.h"
#include <vector>
#include <optional>
#include <memory>

class SignalProcessor {
public:
    SignalProcessor(std::shared_ptr<ITradingPlatform> tp) : tp(tp), signalContainer(SignalContainer::Instance()) {}

    virtual ~SignalProcessor() = default;
    virtual std::optional<std::vector<PendingOrder>> ProcessSignals() = 0;

    virtual std::vector<TradeSignal> GetSignalsToClear() {
        std::vector<TradeSignal> clearedSignals = std::move(signalsToClear);
        signalsToClear.clear();
        return clearedSignals;
    }

protected:
    std::shared_ptr<ITradingPlatform> tp;
    SignalContainer& signalContainer;
    std::vector<TradeSignal> signalsToClear;
};

#endif // SIGNAL_PROCESSOR_H
