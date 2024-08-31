#include "TradeSystem.h"

// Generate pending orders using the SignalManager
std::optional<std::vector<PendingOrder>> TradeSystem::GeneratePendingOrders()
{
    return signalManager_->GeneratePendingOrders();
}

// Audit pending orders using the RiskManager
std::optional<std::vector<PendingOrder>> TradeSystem::RiskAuditPendingOrders(const std::vector<PendingOrder> &pendingOrders)
{
    return riskManager_->AuditPendingOrders(pendingOrders);
}

// Execute pending orders using the OrderManager and update active orders
std::optional<std::vector<ExecutedOrder>> TradeSystem::ExecutePendingOrders(const std::vector<PendingOrder> &pendingOrders)
{
    auto executedOrdersOpt = orderManager_->ExecutePendingOrders(pendingOrders);
    if (executedOrdersOpt.has_value())
    {
        for (const auto &order : executedOrdersOpt.value())
        {
            activeOrders_[order.orderId] = order;
        }
        return executedOrdersOpt;
    }
    return std::nullopt;
}

std::optional<std::vector<PendingOrder>> TradeSystem::FilterPendingOrders(const std::vector<PendingOrder> &pendingOrders)
{
    std::vector<PendingOrder> filteredOrders = pendingOrders;

    if (pendingOrderFilters_.has_value())
    {
        for (const auto &filter : pendingOrderFilters_.value())
        {
            filteredOrders = filter->FilterPendingOrders(filteredOrders);
        }
    }

    return filteredOrders;
}

// Audit active orders using the RiskManager
std::optional<std::vector<ExecutedOrder>> TradeSystem::RiskAuditActiveOrders()
{
    auto activeOrdersVec = GetActiveOrdersAsVector();
    return riskManager_->AuditActiveOrders(activeOrdersVec);
}

// Manage active orders using the OrderManager and update active orders
std::optional<std::vector<ExecutedOrder>> TradeSystem::ManageActiveOrders()
{
    auto activeOrdersVec = GetActiveOrdersAsVector();
    auto managedOrdersOpt = orderManager_->ManageActiveOrders(activeOrdersVec);
    if (managedOrdersOpt.has_value())
    {
        for (const auto &order : managedOrdersOpt.value())
        {
            if (order.status == OrderStatus::Cancelled || order.status == OrderStatus::Rejected)
            {
                activeOrders_.erase(order.orderId);
            }
            else
            {
                activeOrders_[order.orderId] = order;
            }
        }
        return managedOrdersOpt;
    }
    return std::nullopt;
}

// Initialize the ContextManager
void TradeSystem::Initialize()
{
    _putenv("TZ=America/Chicago");
    _tzset(); // Apply the timezone change immediately
    parameterManager_ = ParameterManager::Instance();
    contextManager_->Initialize();
    Logger::Log("TradeSystem initialized", Logger::LogLevel::LOG_INFO);
}

// Main process method to run the trading system
void TradeSystem::Process()
{
    // Handle setting up, ending, updating session
    int throttleMS = 0; // Example to remind us we can set a throttle time.
    if (!contextManager_->ManageSession(throttleMS))
    {
        // If we're not ready for trade, check to see if we can re-initialize trading system.
        // This will only happen if we save a change to trading system from ui. May want to handle this differently
        // Analyze TODO
        if (ParameterManager::Instance()->GetIsTradeSystemStale())
        {
            contextManager_->Initialize();
        }

        return;
    }

    // Audit active orders
    auto auditedActiveOrdersOpt = RiskAuditActiveOrders();
    if (auditedActiveOrdersOpt.has_value())
    {
        auto auditedActiveOrders = auditedActiveOrdersOpt.value();
        for (const auto &order : auditedActiveOrders)
        {
            activeOrders_[order.orderId] = order;
        }
    }

    // Manage active orders
    auto managedOrdersOpt = ManageActiveOrders();
    if (managedOrdersOpt.has_value())
    {
        auto managedOrders = managedOrdersOpt.value();
        for (const auto &order : managedOrders)
        {
            if (order.status == OrderStatus::Filled || order.status == OrderStatus::Cancelled || order.status == OrderStatus::Rejected)
            {
                activeOrders_.erase(order.orderId);
            }
            else
            {
                activeOrders_[order.orderId] = order;
            }
        }
    }

    // Generate pending orders
    auto pendingOrdersOpt = GeneratePendingOrders();
    if (!pendingOrdersOpt.has_value())
    {
        return;
    }
    auto pendingOrders = pendingOrdersOpt.value();

    // Evaluate position risk
    auto currentPosition = tradingPlatform_->GetPositionData();
    auto passedPositionAudit = riskManager_->AuditPosition(currentPosition);
    if (!passedPositionAudit)
    {
        Logger::Log("Position risk is too high. Pausing trading. ", Logger::LogLevel::LOG_WARNING);
        return;
    }

    // Audit pending orders
    auto auditedOrdersOpt = RiskAuditPendingOrders(pendingOrders);
    if (!auditedOrdersOpt.has_value())
    {
        return;
    }
    auto auditedOrders = auditedOrdersOpt.value();

    // Filter pending orders
    auto filteredOrdersOpt = FilterPendingOrders(auditedOrders);
    if (!filteredOrdersOpt.has_value())
    {
        return;
    }
    auto filteredOrders = filteredOrdersOpt.value();

    // Execute pending orders
    auto executedOrdersOpt = ExecutePendingOrders(filteredOrders);
    if (!executedOrdersOpt.has_value())
    {
        return;
    }
    auto executedOrders = executedOrdersOpt.value();
}