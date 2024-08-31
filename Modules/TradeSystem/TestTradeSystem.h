#ifndef TEST_TRADE_SYSTEM_H
#define TEST_TRADE_SYSTEM_H

#include "TradeSystem.h"
#include "SignalManager.h"
#include "RiskManager.h"
#include "OrderManager.h"
#include "ContextManager.h"
#include <memory>
#include <vector>

class TestTradeSystem : public TradeSystem {
public:
    TestTradeSystem(
        const std::shared_ptr<SignalManager>& signalManager,
        const std::shared_ptr<RiskManager>& riskManager,
        const std::shared_ptr<ContextManager>& contextManager,
        const std::shared_ptr<ITradingPlatform>& tradingPlatform,
        const std::shared_ptr<OrderManager>& orderManager)
        : TradeSystem(signalManager, riskManager, contextManager, tradingPlatform, orderManager) {}
};

#endif // TEST_TRADE_SYSTEM_H
