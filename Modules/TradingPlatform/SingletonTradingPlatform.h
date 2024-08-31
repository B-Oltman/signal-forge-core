#ifndef SINGLETON_TRADING_PLATFORM_H
#define SINGLETON_TRADING_PLATFORM_H

#include <memory>
#include <sierrachart.h>
#include "ITradingPlatform.h"
#include "SierraChartPlatform.h"

class SingletonTradingPlatform {
public:
    static std::shared_ptr<ITradingPlatform> Instance(s_sc* sc, ContextType context) {
        if (!instance) {
            instance = std::make_shared<SierraChartPlatform>(sc, context);
        }
        return instance;
    }

    static std::shared_ptr<ITradingPlatform> GetInstance() {
        return instance;
    }

    static void UpdateSCRef(s_sc* sc) {
        auto platform = std::dynamic_pointer_cast<SierraChartPlatform>(instance);
        if (platform) {
            platform->UpdateSCRef(sc);
        }
    }

    SingletonTradingPlatform(const SingletonTradingPlatform&) = delete;
    SingletonTradingPlatform& operator=(const SingletonTradingPlatform&) = delete;

private:
    SingletonTradingPlatform() = default;
    static std::shared_ptr<ITradingPlatform> instance;
};

#endif // SINGLETON_TRADING_PLATFORM_H
