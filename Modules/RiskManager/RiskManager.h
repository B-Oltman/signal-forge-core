#ifndef RISK_MANAGER_H
#define RISK_MANAGER_H

#include "CommonTypes.h"
#include "ITradingPlatform.h"
#include "ActiveOrderRiskProcessor.h"
#include "PendingOrderRiskProcessor.h"
#include "PositionRiskProcessor.h"
#include <memory>

class RiskManager {
public:
    RiskManager(std::shared_ptr<ActiveOrderRiskProcessor> activeOrderRiskProcessor,
                std::shared_ptr<PendingOrderRiskProcessor> pendingOrderRiskProcessor,
                std::shared_ptr<PositionRiskProcessor> positionRiskProcessor,
                std::shared_ptr<ITradingPlatform> tradingPlatform)
        : activeOrderRiskProcessor_(activeOrderRiskProcessor),
          pendingOrderRiskProcessor_(pendingOrderRiskProcessor),
          positionRiskProcessor_(positionRiskProcessor),
          tp(tradingPlatform) {}

    virtual ~RiskManager() = default;

    virtual std::optional<std::vector<PendingOrder>> AuditPendingOrders(const std::vector<PendingOrder>& pendingOrders) = 0;
    virtual std::optional<std::vector<ExecutedOrder>> AuditActiveOrders(const std::vector<ExecutedOrder>& activeOrders) = 0;
    virtual bool AuditPosition(const PositionData& positionData) = 0;

protected:
    std::shared_ptr<ActiveOrderRiskProcessor> activeOrderRiskProcessor_;
    std::shared_ptr<PendingOrderRiskProcessor> pendingOrderRiskProcessor_;    
    std::shared_ptr<PositionRiskProcessor> positionRiskProcessor_;
    std::shared_ptr<ITradingPlatform> tp;
};

#endif // RISK_MANAGER_H
