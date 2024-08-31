#include "DefaultRiskManager.h"

std::optional<std::vector<ExecutedOrder>> DefaultRiskManager::AuditActiveOrders(const std::vector<ExecutedOrder>& activeOrders) {
    std::vector<ExecutedOrder> updatedOrders;
    
    for (const auto& activeOrder : activeOrders) {
        activeOrderRiskProcessor_->EvaluateActiveOrder(activeOrder);

        if (activeOrder.orderRisk.acceptable) {                        
            // Do nothing
        } else {
            // Close or cancel the order
            auto result = tp->CloseOrCancelOrder(activeOrder);
            updatedOrders.push_back(activeOrder);
        }                
    }   

    if (updatedOrders.empty()) {
        return std::nullopt;
    } else {
        return updatedOrders;
    }
}

std::optional<std::vector<PendingOrder>> DefaultRiskManager::AuditPendingOrders(const std::vector<PendingOrder>& pendingOrders) {
    std::vector<PendingOrder> validOrders;
    
    for (const auto& pendingOrder : pendingOrders) {
        pendingOrderRiskProcessor_->EvaluatePendingOrder(pendingOrder);

        if (pendingOrder.orderRisk.acceptable) {
            validOrders.push_back(pendingOrder);
        } else {
            // TODO: Log the rejected order;
        }                
    }   

    if (validOrders.empty()) {
        return std::nullopt;
    } else {
        return validOrders;
    }
}

bool DefaultRiskManager::AuditPosition(const PositionData& positionData) {    
    auto riskAssesment = positionRiskProcessor_->EvaluatePositionRisk(positionData);    
    return riskAssesment.acceptable;
}
