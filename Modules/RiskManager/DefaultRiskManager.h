#ifndef DEFAULT_RISK_MANAGER_H
#define DEFAULT_RISK_MANAGER_H

#include "RiskManager.h"

class DefaultRiskManager : public RiskManager {
public:
    using RiskManager::RiskManager; // Inherit constructors

    std::optional<std::vector<PendingOrder>> AuditPendingOrders(const std::vector<PendingOrder>& pendingOrders) override;
    std::optional<std::vector<ExecutedOrder>> AuditActiveOrders(const std::vector<ExecutedOrder>& activeOrders) override;
    bool AuditPosition(const PositionData& positionData) override;
};

#endif // DEFAULT_RISK_MANAGER_H
