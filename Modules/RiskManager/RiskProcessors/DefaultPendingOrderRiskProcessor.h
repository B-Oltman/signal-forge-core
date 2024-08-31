#ifndef DEFAULT_PENDING_ORDER_RISK_PROCESSOR_H
#define DEFAULT_PENDING_ORDER_RISK_PROCESSOR_H

#include "PendingOrderRiskProcessor.h"

class DefaultPendingOrderRiskProcessor : public PendingOrderRiskProcessor {
public:
    DefaultPendingOrderRiskProcessor(std::shared_ptr<ITradingPlatform> tradingPlatform)
        : PendingOrderRiskProcessor(tradingPlatform) {}

    RiskAssessment EvaluatePendingOrder(const PendingOrder& pendingOrder) const override; 
};

#endif // DEFAULT_PENDING_ORDER_RISK_PROCESSOR_H
