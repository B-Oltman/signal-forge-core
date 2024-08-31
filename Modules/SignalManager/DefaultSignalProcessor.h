#ifndef DEFAULT_SIGNAL_PROCESSOR_H
#define DEFAULT_SIGNAL_PROCESSOR_H

#include "SignalProcessor.h"
#include "SignalContainer.h"

class DefaultSignalProcessor : public SignalProcessor {
public:
    DefaultSignalProcessor(std::shared_ptr<ITradingPlatform> tp) : SignalProcessor(tp) {}

    std::optional<std::vector<PendingOrder>> ProcessSignals() override {
        std::vector<PendingOrder> pendingOrders;

        // Example logic for processing signals and creating pending orders
        auto signal2Opt = signalContainer.GetSignalById("SIGNAL_2");

        if (signal2Opt.has_value()) {
            auto signal2 = signal2Opt.value();
            for (const auto& attachedId : signal2.attachedSignalIds) {
                auto signal1Opt = signalContainer.GetSignalById(attachedId);
                if (signal1Opt.has_value()) {
                    auto signal1 = signal1Opt.value();

                    PendingOrder order;
                    order.signalWeight = 1; // Example weight
                    order.symbol = "SYMBOL"; // Example symbol
                    order.quantity = 1; // Example quantity
                    order.price = signal1.price; // Example price from signal 1

                    pendingOrders.push_back(order);

                    // Mark the processed signals for clearing
                    signalsToClear.push_back(signal1);
                }
            }

            signalsToClear.push_back(signal2);
        }

        if (!pendingOrders.empty()) {
            return pendingOrders;
        }
        return std::nullopt;
    }

private:
    std::vector<TradeSignal> signalsToClear;
};

#endif // DEFAULT_SIGNAL_PROCESSOR_H
