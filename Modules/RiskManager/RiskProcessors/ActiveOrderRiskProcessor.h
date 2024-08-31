#ifndef ACTIVE_ORDER_RISK_PROCESSOR_H
#define ACTIVE_ORDER_RISK_PROCESSOR_H

#include "CommonTypes.h"
#include "ITradingPlatform.h"
#include <memory>

class ActiveOrderRiskProcessor {
public:
    ActiveOrderRiskProcessor(std::shared_ptr<ITradingPlatform> tradingPlatform)
        : tp(tradingPlatform) {}

    virtual ~ActiveOrderRiskProcessor() = default;

    // Pure virtual function to be implemented by derived classes
    virtual RiskAssessment EvaluateActiveOrder(const ExecutedOrder& activeOrder) const = 0;

protected:
    std::shared_ptr<ITradingPlatform> tp; // Shared pointer to the trading platform
};

#endif // ACTIVE_ORDER_RISK_PROCESSOR_H
