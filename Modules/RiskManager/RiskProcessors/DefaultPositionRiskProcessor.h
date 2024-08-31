#ifndef DEFAULT_POSITION_RISK_PROCESSOR_H
#define DEFAULT_POSITION_RISK_PROCESSOR_H

#include "PositionRiskProcessor.h"

class DefaultPositionRiskProcessor : public PositionRiskProcessor {
public:
    DefaultPositionRiskProcessor(std::shared_ptr<ITradingPlatform> tradingPlatform)
        : PositionRiskProcessor(tradingPlatform) {}

    RiskAssessment EvaluatePositionRisk(const PositionData& positionData) const override;
};

#endif // DEFAULT_POSITION_RISK_PROCESSOR_H
