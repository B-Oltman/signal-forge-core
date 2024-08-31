#ifndef DEFAULT_ORDER_MANAGER_H
#define DEFAULT_ORDER_MANAGER_H

#include "OrderManager.h"
#include <unordered_map>
#include <vector>

class DefaultOrderManager : public OrderManager {
public:
    DefaultOrderManager(std::shared_ptr<IOrderProcessor> orderProcessor, std::shared_ptr<ITradingPlatform> tradingPlatform)
        : OrderManager(orderProcessor, tradingPlatform) {}

    std::optional<std::vector<ExecutedOrder>> ExecutePendingOrders(const std::vector<PendingOrder>& pendingOrders) override {
        std::vector<ExecutedOrder> executedOrders;

        for (const auto& pendingOrder : pendingOrders) {
            auto result = orderProcessor_->ProcessPendingOrder(pendingOrder, tp);
            if (result.has_value()) {
                auto orders = result.value();
                executedOrders.insert(executedOrders.end(), orders.begin(), orders.end());
            }
        }

        if (!executedOrders.empty()) {
            return executedOrders;
        }
        return std::nullopt;
    }

    std::optional<std::vector<ExecutedOrder>> ManageActiveOrders(const std::vector<ExecutedOrder>& activeOrders) override {
        std::vector<ExecutedOrder> managedOrders;

        for (const auto& activeOrder : activeOrders) {
            auto result = orderProcessor_->ProcessActiveOrder(activeOrder, tp);
            if (result.has_value()) {
                auto orders = result.value();
                managedOrders.insert(managedOrders.end(), orders.begin(), orders.end());
            }
        }

        if (!managedOrders.empty()) {
            return managedOrders;
        }
        return std::nullopt;
    }
};

#endif // DEFAULT_ORDER_MANAGER_H
