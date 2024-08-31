#pragma once

#include "CommonTypes.h"
#include "ITradingPlatform.h"
#include <optional>
#include <vector>

class IOrderProcessor {
public:
    virtual ~IOrderProcessor() = default;
    // This will use the OrderManager to execute the pendingOrders and return the executed orders.
    virtual std::optional<std::vector<ExecutedOrder>> ProcessPendingOrder(const PendingOrder& pendingOrder, std::shared_ptr<ITradingPlatform> tp) = 0;

    // This will use the OrderManager to manage the active orders remaining after audit. It will return any orders that have had a status change during the current cycle.
    virtual std::optional<std::vector<ExecutedOrder>> ProcessActiveOrder(const ExecutedOrder& activeOrder, std::shared_ptr<ITradingPlatform> tp) = 0;
};
