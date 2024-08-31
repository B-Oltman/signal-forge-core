#ifndef DEFAULT_ACTIVE_ORDER_RISK_PROCESSOR_H
#define DEFAULT_ACTIVE_ORDER_RISK_PROCESSOR_H

#include "ActiveOrderRiskProcessor.h"

class DefaultActiveOrderRiskProcessor : public ActiveOrderRiskProcessor {
public:
    DefaultActiveOrderRiskProcessor(std::shared_ptr<ITradingPlatform> tradingPlatform)
        : ActiveOrderRiskProcessor(tradingPlatform) {}

    RiskAssessment EvaluateActiveOrder(const ExecutedOrder& activeOrder) const override;
};

#endif // DEFAULT_ACTIVE_ORDER_RISK_PROCESSOR_H
