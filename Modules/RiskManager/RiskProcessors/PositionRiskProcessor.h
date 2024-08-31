#ifndef POSITION_RISK_PROCESSOR_H
#define POSITION_RISK_PROCESSOR_H

#include "CommonTypes.h"
#include "ITradingPlatform.h"
#include <memory>

class PositionRiskProcessor {
public:
    PositionRiskProcessor(std::shared_ptr<ITradingPlatform> tradingPlatform)
        : tp(tradingPlatform) {}

    virtual ~PositionRiskProcessor() = default;

    // Pure virtual function to be implemented by derived classes
    virtual RiskAssessment EvaluatePositionRisk(const PositionData& positionData) const = 0;

protected:
    std::shared_ptr<ITradingPlatform> tp; // Shared pointer to the trading platform
};

#endif // POSITION_RISK_PROCESSOR_H
