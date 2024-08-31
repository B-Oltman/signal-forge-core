#ifndef ORDER_MANAGER_H
#define ORDER_MANAGER_H

#include "CommonTypes.h"
#include "IOrderProcessor.h"
#include "ITradingPlatform.h"
#include <vector>
#include <memory>

class OrderManager {
public:
    OrderManager(std::shared_ptr<IOrderProcessor> orderProcessor, std::shared_ptr<ITradingPlatform> tradingPlatform)
    : orderProcessor_(orderProcessor), tp(tradingPlatform) {}
    
    virtual ~OrderManager() = default;
    
    // This will use the OrderManager to execute the pendingOrders and return the executed orders.
    virtual std::optional<std::vector<ExecutedOrder>> ExecutePendingOrders(const std::vector<PendingOrder>& pendingOrders) = 0;

    // This will use the OrderManager to manage the active orders remaining after audit. It will return any orders that have had a status change during the current cycle.
    virtual std::optional<std::vector<ExecutedOrder>> ManageActiveOrders(const std::vector<ExecutedOrder>& activeOrders) = 0;

protected:
  std::shared_ptr<IOrderProcessor> orderProcessor_;
  std::shared_ptr<ITradingPlatform> tp;
};

#endif // ORDER_MANAGER_H
