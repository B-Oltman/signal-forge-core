#ifndef TRADE_SYSTEM_BUILDER_H
#define TRADE_SYSTEM_BUILDER_H

#include <memory>
#include <stdexcept>
#include "CommonTypes.h"

// Include components
#include "TradeSystem.h"
#include "SingletonTradingPlatform.h"
#include "SignalManager.h"
#include "LevelManager/LevelManager.h"
#include "LevelManager/LevelGenerator.h"
#include "LevelManager/LevelProcessor.h"
#include "SignalProcessor.h"
#include "SignalGenerator.h"
#include "RiskManager.h"
#include "ActiveOrderRiskProcessor.h"
#include "PendingOrderRiskProcessor.h"
#include "PositionRiskProcessor.h"
#include "OrderManager.h"
#include "IOrderProcessor.h"
#include "ContextManager.h"
#include "IPendingOrderFilter.h"

// Include default implementations for the components
#include "DefaultSignalManager.h"
#include "DefaultRiskManager.h"
#include "DefaultOrderManager.h"
#include "DefaultActiveOrderRiskProcessor.h"
#include "DefaultPendingOrderRiskProcessor.h"
#include "DefaultPositionRiskProcessor.h"
#include "DefaultOrderProcessor.h"
#include "DefaultSignalProcessor.h"
#include "DefaultSignalGenerator.h"
#include "LevelManager/DefaultLevelProcessor.h"
#include "LevelManager/DefaultLevelGenerator.h"

template <typename TTradeSystem>
class TradeSystemBuilder
{
    static_assert(std::is_base_of<TradeSystem, TTradeSystem>::value, "TTradeSystem must derive from TradeSystem");

public:
    TradeSystemBuilder(s_sc* sc, ContextType context)
    {
        tradingPlatform_ = SingletonTradingPlatform::Instance(sc, context);
    }

    TradeSystemBuilder &WithSignalManager(std::shared_ptr<SignalManager> signalManager)
    {
        signalManager_ = signalManager;
        return *this;
    }

    template <typename TSignalGenerator>
    TradeSystemBuilder &WithSignalGenerator()
    {
        static_assert(std::is_base_of<SignalGenerator, TSignalGenerator>::value, "TSignalGenerator must derive from SignalGenerator");
        signalGenerator_ = std::make_shared<TSignalGenerator>(tradingPlatform_);
        return *this;
    }

    template <typename TSignalProcessor>
    TradeSystemBuilder &WithSignalProcessor()
    {
        static_assert(std::is_base_of<SignalProcessor, TSignalProcessor>::value, "TSignalProcessor must derive from SignalProcessor");
        signalProcessor_ = std::make_shared<TSignalProcessor>(tradingPlatform_);
        return *this;
    }

    TradeSystemBuilder &WithLevelManager(std::shared_ptr<LevelManager> levelManager)
    {
        levelManager_ = levelManager;
        return *this;
    }

    template <typename TLevelGenerator>
    TradeSystemBuilder &WithLevelGenerator()
    {
        static_assert(std::is_base_of<LevelGenerator, TLevelGenerator>::value, "TLevelGenerator must derive from LevelGenerator");
        if (!levelGenerators_)
        {
            levelGenerators_ = std::make_shared<std::vector<std::shared_ptr<LevelGenerator>>>();
        }
        levelGenerators_->push_back(std::make_shared<TLevelGenerator>(tradingPlatform_));
        includeLevelManager = true;
        return *this;
    }

    template <typename TLevelProcessor>
    TradeSystemBuilder &WithLevelProcessor()
    {
        static_assert(std::is_base_of<LevelProcessor, TLevelProcessor>::value, "TLevelProcessor must derive from LevelProcessor");
        levelProcessor_ = std::make_shared<TLevelProcessor>(tradingPlatform_);
        includeLevelManager = true;
        return *this;
    }

    template <typename TTuningDataProcessor>
    TradeSystemBuilder &WithTuningDataProcessor()
    {
        static_assert(std::is_base_of<ITuningDataProcessor, TTuningDataProcessor>::value, "TTuningDataProcessor must derive from ITuningDataProcessor");
        parameterTuner_ = std::make_shared<TTuningDataProcessor>();
        return *this;
    }

    template <typename TActiveOrderRiskProcessor>
    TradeSystemBuilder &WithActiveOrderRiskProcessor()
    {
        static_assert(std::is_base_of<ActiveOrderRiskProcessor, TActiveOrderRiskProcessor>::value, "TActiveOrderRiskProcessor must derive from ActiveOrderRiskProcessor");
        activeOrderRiskProcessor_ = std::make_shared<TActiveOrderRiskProcessor>(tradingPlatform_);
        return *this;
    }

    template <typename TPendingOrderRiskProcessor>
    TradeSystemBuilder &WithPendingOrderRiskProcessor()
    {
        static_assert(std::is_base_of<PendingOrderRiskProcessor, TPendingOrderRiskProcessor>::value, "TPendingOrderRiskProcessor must derive from PendingOrderRiskProcessor");
        pendingOrderRiskProcessor_ = std::make_shared<TPendingOrderRiskProcessor>(tradingPlatform_);
        return *this;
    }

    template <typename TPositionRiskProcessor>
    TradeSystemBuilder &WithPositionRiskProcessor()
    {
        static_assert(std::is_base_of<PositionRiskProcessor, TPositionRiskProcessor>::value, "TPositionRiskProcessor must derive from PositionRiskProcessor");
        positionRiskProcessor_ = std::make_shared<TPositionRiskProcessor>(tradingPlatform_);
        return *this;
    }

    TradeSystemBuilder &WithRiskManager(std::shared_ptr<RiskManager> riskManager)
    {
        riskManager_ = riskManager;
        return *this;
    }

    template <typename TOrderProcessor>
    TradeSystemBuilder &WithOrderProcessor()
    {
        static_assert(std::is_base_of<IOrderProcessor, TOrderProcessor>::value, "TOrderProcessor must derive from IOrderProcessor");
        orderProcessor_ = std::make_shared<TOrderProcessor>();
        return *this;
    }

    TradeSystemBuilder &WithOrderManager(std::shared_ptr<OrderManager> orderManager)
    {
        orderManager_ = orderManager;
        return *this;
    }

    template <typename TPendingOrderFilter>
    TradeSystemBuilder &WithPendingOrderFilter()
    {
        static_assert(std::is_base_of<IPendingOrderFilter, TPendingOrderFilter>::value, "TPendingOrderFilter must derive from IPendingOrderFilter");
        if (!pendingOrderFilters_)
        {
            pendingOrderFilters_ = std::make_optional<std::vector<std::shared_ptr<IPendingOrderFilter>>>();
        }
        pendingOrderFilters_->push_back(std::make_shared<TPendingOrderFilter>());
        return *this;
    }

    std::shared_ptr<TTradeSystem> Build(std::string systemName)
    {
        ValidateAndSetDefaults();
        contextManager_ = std::make_shared<ContextManager>(tradingPlatform_, systemName, parameterTuner_);

        return std::make_shared<TTradeSystem>(
            signalManager_,
            riskManager_,
            contextManager_,
            tradingPlatform_,
            orderManager_,
            pendingOrderFilters_);
    }

private:
    void ValidateAndSetDefaults()
    {
        if (!signalManager_)
        {
            if (!signalGenerator_)
            {
                signalGenerator_ = std::make_shared<DefaultSignalGenerator>(tradingPlatform_);
            }
            if (!signalProcessor_)
            {
                signalProcessor_ = std::make_shared<DefaultSignalProcessor>(tradingPlatform_);
            }

            if (includeLevelManager && !levelManager_)
            {
                if (!levelProcessor_)
                {
                    levelProcessor_ = std::make_shared<DefaultLevelProcessor>(tradingPlatform_);
                }

                if (!levelGenerators_)
                {
                    levelGenerators_ = std::make_shared<std::vector<std::shared_ptr<LevelGenerator>>>();
                    levelGenerators_->push_back(std::make_shared<DefaultLevelGenerator>(tradingPlatform_));
                }

                levelManager_ = std::make_shared<LevelManager>(*levelGenerators_, levelProcessor_, tradingPlatform_);
            }

            // Convert optional to shared_ptr
            std::shared_ptr<LevelManager> levelManagerPtr = levelManager_.value_or(nullptr);

            signalManager_ = std::make_shared<DefaultSignalManager>(signalGenerator_, signalProcessor_, tradingPlatform_, levelManagerPtr);
        }

        if (!riskManager_)
        {
            if (!activeOrderRiskProcessor_)
            {
                activeOrderRiskProcessor_ = std::make_shared<DefaultActiveOrderRiskProcessor>(tradingPlatform_);
            }
            if (!pendingOrderRiskProcessor_)
            {
                pendingOrderRiskProcessor_ = std::make_shared<DefaultPendingOrderRiskProcessor>(tradingPlatform_);
            }
            if (!positionRiskProcessor_)
            {
                positionRiskProcessor_ = std::make_shared<DefaultPositionRiskProcessor>(tradingPlatform_);
            }

            riskManager_ = std::make_shared<DefaultRiskManager>(activeOrderRiskProcessor_, pendingOrderRiskProcessor_, positionRiskProcessor_, tradingPlatform_);
        }

        if (!orderManager_)
        {
            if (!orderProcessor_)
            {
                orderProcessor_ = std::make_shared<DefaultOrderProcessor>();
            }

            orderManager_ = std::make_shared<DefaultOrderManager>(orderProcessor_, tradingPlatform_);
        }
    }

    bool includeLevelManager = false;
    std::shared_ptr<SignalManager> signalManager_;
    std::shared_ptr<SignalGenerator> signalGenerator_;
    std::shared_ptr<SignalProcessor> signalProcessor_;
    std::shared_ptr<ActiveOrderRiskProcessor> activeOrderRiskProcessor_;
    std::shared_ptr<PendingOrderRiskProcessor> pendingOrderRiskProcessor_;
    std::shared_ptr<PositionRiskProcessor> positionRiskProcessor_;
    std::shared_ptr<RiskManager> riskManager_;
    std::shared_ptr<IOrderProcessor> orderProcessor_;
    std::shared_ptr<OrderManager> orderManager_;
    std::shared_ptr<ContextManager> contextManager_;
    std::shared_ptr<ITradingPlatform> tradingPlatform_;
    std::optional<std::shared_ptr<LevelManager>> levelManager_;
    std::optional<std::vector<std::shared_ptr<IPendingOrderFilter>>> pendingOrderFilters_;
    std::optional<std::shared_ptr<ITuningDataProcessor>> parameterTuner_;
    std::shared_ptr<std::vector<std::shared_ptr<LevelGenerator>>> levelGenerators_;
    std::shared_ptr<LevelProcessor> levelProcessor_;
};

#endif // TRADE_SYSTEM_BUILDER_H
