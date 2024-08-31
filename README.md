# Quant Framework ReadMe

All rights reserved Baruch Oltman

---

## Overview

The Quant Framework is designed to provide a modular and extensible architecture for building trading systems. The framework includes components for signal generation, risk management, order execution, order management, market context analysis, and trading context setup. This framework can be integrated with any trading platform that can provide the needed functionality to implement the `ITradingPlatform` interface. The example provided here demonstrates integration with SierraChart as a custom study.

### Key Components:

- **TradeSystem**: Manages the overall trading logic, integrating signals, risk assessment, order execution, and management.
- **SignalGenerator**: Generates trade signals based on market data.
- **RiskManager**: Evaluates the risk of trade signals and active orders.
- **OrderExecutor**: Executes orders based on trade signals.
- **OrderManager**: Manages active orders and their lifecycle.
- **MarketContext**: Provides market data and context for trading decisions.
- **TradingContext**: Sets up and manages the trading environment.
- **BacktestingManager**: Handles backtesting of trading strategies.
- **Calculations**: Contains various mathematical and statistical functions used in the system.
- **CommonTypes**: Defines common data types and structures used across the system.
- **ConfigManager**: Manages configuration files and parameters.
- **FileIO**: Handles file input and output operations.
- **HttpClient**: Manages HTTP requests for data retrieval.
- **Logger**: Provides logging functionality for debugging and monitoring.
- **MLModelTraining**: Manages training of machine learning models for trading signals.
- **MLSignalGenerator**: Generates trade signals using machine learning models.
- **Profiling**: Provides performance profiling tools.
- **Timing**: Manages timing functions and operations within the system.
- **Messaging**: Facilitates inter-module communication through a message-passing system.
- **LevelManager**: Manages the generation and processing of price levels.
- **LevelGenerator**: Generates price levels based on current market data.
- **LevelProcessor**: Processes the generated levels to produce trading signals.
- **MLTuningManager**: Manages the performance tuning and evaluation of parameters used in trading strategies.

### Important Note:

The provided `DefaultTradeSystem` and other components with default implementations are examples. It is expected that users will provide their own implementations tailored to their specific trading strategies and requirements.

## Usage

### Integration with Different Trading Platforms

The platform is designed to support different trading platforms. The example below shows usage with SierraChart, but you can use different platforms by providing your own implementation of the `ITradingPlatform` interface and initializing that in the Singleton.

### Example with SierraChart:

1. **Initialize the Trade System**:
   - The `TradeSystem` should be initialized within a SierraChart study entry point. SierraChart will call the study entry point repeatedly, allowing the `TradeSystem` to process signals and manage orders in each call.

```cpp
#include <sierrachart.h>
#include "OpposingHammerTradeSystem.h"
#include "OpposingHammerSignalGenerator.h"
#include "OpposingHammerSignalProcessor.h"
#include "OpposingHammerActiveOrderRiskProcessor.h"
#include "OpposingHammerPendingOrderRiskProcessor.h"
#include "OpposingHammerPositionRiskProcessor.h"
#include "OpposingHammerOrderProcessor.h"
#include "OpposingHammerMarketTimingOrderFilter.h"
#include "OpposingHammerLevelGenerator.h"
#include "OpposingHammerLevelProcessor.h"
#include "OpposingHammerParameterTuner.h"
#include "ParameterManager.h"
#include "TradeSystemBuilder.h"
#include "SingletonTradingPlatform.h"
#include <memory>

SCDLLName("OpposingHammerTradeSystem")

// Main entry point for the Opposing Hammer Trade System study
SCSFExport scsf_OpposingHammerTradeSystem(SCStudyInterfaceRef sc)
{
    static std::shared_ptr<OpposingHammerTradeSystem> tradeSystem;

    // Set default settings for the study
    if (sc.SetDefaults)
    {
        sc.GraphName = "Opposing Hammer Trade System"; // Name of the study
        sc.AutoLoop = 1; // Enable autoloop
        return;
    }

    if (sc.LastCallToFunction)
    {
        // Reset the smart pointer to destruct the trade system.
        tradeSystem.reset();
        return;
    }

    // Initialize the trade system if it's not already created
    if (!tradeSystem)
    {                
        // Build the trade system using the TradeSystemBuilder
        tradeSystem = TradeSystemBuilder<OpposingHammerTradeSystem>(sc)
            .WithSignalGenerator<OpposingHammerSignalGenerator>() // Set custom signal generator
            .WithSignalProcessor<OpposingHammerSignalProcessor>() // Set custom signal processor
            .WithActiveOrderRiskProcessor<OpposingHammerActiveOrderRiskProcessor>() // Set custom active order risk processor
            .WithPendingOrderRiskProcessor<OpposingHammerPendingOrderRiskProcessor>() // Set custom pending order risk processor
            .WithPositionRiskProcessor<OpposingHammerPositionRiskProcessor>() // Set custom system-wide risk processor
            .WithOrderProcessor<OpposingHammerOrderProcessor>() // Set custom order processor
            .WithPendingOrderFilter<OpposingHammerMarketTimingOrderFilter>() // Set custom market timing order filter
            .WithTuningDataProcessor<OpposingHammerParameterTuner>() // Set custom parameter tuner
            .WithLevelGenerator<OpposingHammerLevelGenerator>() // Set custom level generator
            .WithLevelProcessor<OpposingHammerLevelProcessor>() // Set custom level processor
            .Build(ContextType::Backtesting, "./tradeSystemConfigs/opposingHammerConfig.json"); // Build for backtesting context

        // Initialize the trade system
        tradeSystem->Initialize();
    }

    // Process the trade system
    tradeSystem->Process();
}
```

### Messaging System

The `FileIO`, `MarketContext`, and `MLTuningManager` modules use a messaging system to support high-performance trading systems. This design ensures that data fetching and processing are non-blocking and efficient, crucial for high-frequency trading (HFT) strategies.

#### FileIO Messaging:
- Handles asynchronous file operations, ensuring the main trading loop is not blocked.

#### MarketContext Messaging:
- Fetches market data asynchronously using the `HttpClient` module, allowing for real-time data processing without blocking the main trading logic.

#### MLTuningManager Messaging:
- Manages performance tuning and evaluation of trading parameters. It uses the messaging system to asynchronously process tuning data, ensuring that the main trading operations are not interrupted. The `MLTuningManager` subscribes to messages that contain performance data, processes the data, and sends optimized parameters back to the trading system.

## Modules Overview

### BacktestingManager
- Handles backtesting of trading strategies to evaluate performance based on historical data.

### Calculations
- Provides mathematical and statistical functions necessary for trading strategies.

### CommonTypes
- Defines common data types and structures used across the framework.

### ConfigManager
- Manages configuration files and parameters for the system.

### ContextManager
- Manages the context in which trading operations are executed.

### FileIO
- Handles file input and output operations, including asynchronous file operations.

### HttpClient
- Manages HTTP requests for retrieving market data and other necessary information.

### Logger
- Provides logging capabilities for debugging and system monitoring.

### MarketContext
- Provides market data and context necessary for making trading decisions.

### MLModelTraining
- Manages the training process of machine learning models used for generating trade signals.

### MLSignalGenerator
- Generates trade signals based on trained machine learning models.

### OrderExecutor
- Executes orders based on the signals generated by the SignalGenerator.

### OrderManager
- Manages the lifecycle of active orders, including tracking, updating, and closing orders.

### ParameterManager
- Manages parameters and configurations for trading and system setup.

### MLTuningManager
- Manages the performance tuning and evaluation of parameters used in trading strategies.

### Profiling
- Provides tools for performance profiling and analysis.

### RiskManager
- Evaluates and manages the risk associated with trade signals and active orders.

### SignalGenerator
- Generates trade signals based on market data and defined algorithms.

### Timing
- Manages timing functions and operations within the trading system.

### TradeSystem
- Coordinates the overall trading process, integrating signals, risk management, order execution, and order management.

### TradingContext
- Sets up and manages the trading environment, including initialization and runtime management.

### LevelManager
- Manages the generation and processing of price levels within the trading system.

### LevelGenerator
- Generates price levels based on current market data.

### LevelProcessor
- Processes the generated levels to produce trading signals.

## Conclusion

This framework provides a robust foundation for building custom trading systems. While the default implementations are provided as examples, it is expected that you will extend and customize these components to fit your specific needs.

## License

All rights reserved Baruch Oltman. This is a closed source project. Do not distribute, modify, or contribute without explicit permission.