#ifndef DEFAULT_SIGNAL_GENERATOR_H
#define DEFAULT_SIGNAL_GENERATOR_H

#include "SignalGenerator.h"
#include "ITradingPlatform.h"   
#include "SignalManager.h"

class DefaultSignalGenerator : public SignalGenerator {
public:
    DefaultSignalGenerator(std::shared_ptr<ITradingPlatform> tp): SignalGenerator(tp) {};

    std::vector<TradeSignal> GenerateSignals() override {
        std::vector<TradeSignal> newSignals;

        // Generate TradeSignal 1
        if (conditionToGenerateSignal1()) {
            TradeSignal signal1;
            signal1.signalKey = "SIGNAL_1";
            newSignals.push_back(signal1);            
        }

        // Generate TradeSignal 2 associated with TradeSignal 1
        auto signal1Opt = signalContainer.GetSignal("SIGNAL_1");
        if (signal1Opt.has_value() && conditionToGenerateSignal2()) {
            TradeSignal signal2;
            signal2.signalKey = "SIGNAL_2";
            signal2.attachedSignalIds.push_back(signal1Opt->id);
            newSignals.push_back(signal2);            
        }

        return newSignals;
    }

private:
    bool conditionToGenerateSignal1() {
        // Implement the condition for generating TradeSignal 1
        return true;
    }

    bool conditionToGenerateSignal2() {
        // Implement the condition for generating TradeSignal 2
        return true;
    }
};

#endif // DEFAULT_SIGNAL_GENERATOR_H
