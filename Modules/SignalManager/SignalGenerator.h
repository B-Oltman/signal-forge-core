#ifndef SIGNAL_GENERATOR_H
#define SIGNAL_GENERATOR_H

#include "CommonTypes.h"
#include "SignalContainer.h"
#include "ITradingPlatform.h"
#include <vector>

class SignalManager;

class SignalGenerator {
public:
    SignalGenerator(std::shared_ptr<ITradingPlatform> tp) : tp(tp), signalContainer(SignalContainer::Instance()) {}

    virtual ~SignalGenerator() = default;
    virtual std::vector<TradeSignal> GenerateSignals() = 0;

protected:
    SignalContainer& signalContainer;
    std::shared_ptr<ITradingPlatform> tp;
};

#endif // SIGNAL_GENERATOR_H
