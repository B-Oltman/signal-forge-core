#ifndef TRADE_SYSTEM_H
#define TRADE_SYSTEM_H

#include "SignalManager.h"
#include "RiskManager.h"
#include "OrderManager.h"
#include "ITradingPlatform.h"
#include "IPendingOrderFilter.h"
#include "ContextManager.h"
#include "CommonTypes.h"
#include <unordered_map>
#include <vector>
#include <memory>

class TradeSystem {
public:
    // Constructor
    TradeSystem(const std::shared_ptr<SignalManager> &signalManager,
                const std::shared_ptr<RiskManager> &riskManager,
                const std::shared_ptr<ContextManager> &contextManager,
                const std::shared_ptr<ITradingPlatform> &tradingPlatform,
                const std::shared_ptr<OrderManager> &orderManager,
                std::optional<std::vector<std::shared_ptr<IPendingOrderFilter>>> pendingOrderFilters = std::nullopt)
        : signalManager_(signalManager),
          riskManager_(riskManager),
          contextManager_(contextManager),
          tradingPlatform_(tradingPlatform),
          orderManager_(orderManager),
          pendingOrderFilters_(pendingOrderFilters)
    {        
    }

    // Destructor
    virtual ~TradeSystem() {
        signalManager_.reset();
        riskManager_.reset();
        contextManager_.reset();
        orderManager_.reset();
        pendingOrderFilters_.reset();
        tradingPlatform_.reset();
        parameterManager_->~ParameterManager();
        parameterManager_.reset();                                
    }

    void SaveLiveResultsSnapshot();

    // Generates pending orders using the SignalManager
    virtual std::optional<std::vector<PendingOrder>> GeneratePendingOrders();

    // Audits pending orders using the RiskManager
    virtual std::optional<std::vector<PendingOrder>> RiskAuditPendingOrders(const std::vector<PendingOrder>& pendingOrders);

    // Filters pending orders using the pending order filters. Can be used for situations such as volatility filtering, time of day filtering etc.
    // Can remove any orders from the execution pipeline beyond what the risk auditor will remove.
    virtual std::optional<std::vector<PendingOrder>> FilterPendingOrders(const std::vector<PendingOrder>& pendingOrders);

    // Executes pending orders using the OrderManager and updates active orders
    virtual std::optional<std::vector<ExecutedOrder>> ExecutePendingOrders(const std::vector<PendingOrder>& pendingOrders);

    // Audits active orders using the RiskManager
    virtual std::optional<std::vector<ExecutedOrder>> RiskAuditActiveOrders();

    // Manages active orders using the OrderManager and updates active orders
    virtual std::optional<std::vector<ExecutedOrder>> ManageActiveOrders();

    // Initializes the ContextManager
    virtual void Initialize();

    // Main process method to run the trading system
    virtual void Process();

    std::shared_ptr<ITradingPlatform> tradingPlatform_;

protected:
    // Converts the active orders map to a vector
    std::vector<ExecutedOrder> GetActiveOrdersAsVector() const {
        std::vector<ExecutedOrder> orders;
        for (const auto& pair : activeOrders_) {
            orders.push_back(pair.second);
        }
        return orders;
    }

    std::shared_ptr<SignalManager> signalManager_;
    std::shared_ptr<RiskManager> riskManager_;
    std::shared_ptr<ContextManager> contextManager_;
    std::shared_ptr<OrderManager> orderManager_;
    std::shared_ptr<ParameterManager> parameterManager_;
    std::optional<std::vector<std::shared_ptr<IPendingOrderFilter>>> pendingOrderFilters_;      

    std::unordered_map<int, ExecutedOrder> activeOrders_;
};

#endif // TRADE_SYSTEM_H