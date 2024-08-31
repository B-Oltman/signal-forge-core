#ifndef PENDING_ORDER_RISK_PROCESSOR_H
#define PENDING_ORDER_RISK_PROCESSOR_H

#include "CommonTypes.h"
#include "ITradingPlatform.h"
#include <memory>

class PendingOrderRiskProcessor {
public:
    PendingOrderRiskProcessor(std::shared_ptr<ITradingPlatform> tradingPlatform)
        : tp(tradingPlatform) {}

    virtual ~PendingOrderRiskProcessor() = default;

    // Pure virtual function to be implemented by derived classes
    virtual RiskAssessment EvaluatePendingOrder(const PendingOrder& pendingOrder) const = 0;

protected:
    std::shared_ptr<ITradingPlatform> tp; // Shared pointer to the trading platform
};

#endif // PENDING_ORDER_RISK_PROCESSOR_H
