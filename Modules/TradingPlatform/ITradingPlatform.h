#pragma once

#include "CommonTypes.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <optional>

class ITradingPlatform {
public:
    virtual ~ITradingPlatform() = default;

    // Message Functions
    // Adds a message to the platform log
    // Parameters:
    // - message: The message to log
    // - showLog: A boolean to indicate if the log should be shown immediately
    virtual void AddMessageToLog(const std::string& message, bool showLog) = 0;

    // Trade Functions
    // Places a buy order
    // Parameters:
    // - pendingOrder: The order details
    // Returns: The executed order details if successful
    virtual std::optional<ExecutedOrder> BuyEntry(PendingOrder pendingOrder) = 0;

    // Places a sell order
    // Parameters:
    // - pendingOrder: The order details
    // Returns: The executed order details if successful
    virtual std::optional<ExecutedOrder> SellEntry(PendingOrder pendingOrder) = 0;

    // Closes a buy order
    // Parameters:
    // - activeOrder: The active order to close
    // Returns: The executed order details if successful
    virtual std::optional<ExecutedOrder> BuyExit(ExecutedOrder activeOrder) = 0;

    // Closes a sell order
    // Parameters:
    // - activeOrder: The active order to close
    // Returns: The executed order details if successful
    virtual std::optional<ExecutedOrder> SellExit(ExecutedOrder activeOrder) = 0;

    // Closes or cancels an active order
    // Parameters:
    // - activeOrder: The active order to close or cancel
    // Returns: The executed order details if successful
    virtual std::optional<ExecutedOrder> CloseOrCancelOrder(ExecutedOrder activeOrder) = 0;

    // OCO Order Functions
    // Submits an OCO (One Cancels Other) order
    // Parameters:
    // - pendingOrder: The base order details
    // - price1, price2: The target prices
    // - target1Offset, stop1Offset: The offsets for the first target and stop
    // - target2Offset, stop2Price: The optional second target offset and stop price
    // Returns: The executed orders if successful
    virtual std::optional<std::vector<ExecutedOrder>> SubmitOCOOrder(PendingOrder pendingOrder, double price1, double price2, double target1Offset, double stop1Offset, double target2Offset = 0, double stop2Price = 0) = 0;

    // Data Retrieval Functions
    // Retrieves bar data by offset from the current bar
    // Parameters:
    // - offsetFromCurrentBar: The offset from the current bar
    // Returns: The bar data if found
    virtual std::optional<BarData> GetBarByOffsetFromCurrent(int offsetFromCurrentBar = 0) const = 0;

    // Retrieves bar data by time
    // Parameters:
    // - dateTime: The time of the bar
    // Returns: The bar data if found
    virtual std::optional<BarData> GetBarByTime(const DateTime& dateTime) const = 0;

    // Retrieves a range of bar data by offset
    // Parameters:
    // - startOffset, endOffset: The start and end offsets
    // Returns: The bar data in the range
    virtual std::vector<BarData> GetBarRangeByOffset(int startOffset, int endOffset) const = 0;

    // Retrieves a range of bar data by time
    // Parameters:
    // - startDateTime, endDateTime: The start and end times
    // Returns: The bar data in the range
    virtual std::vector<BarData> GetBarRangeByTime(const DateTime& startDateTime, const DateTime& endDateTime) const = 0;

    // Retrieves bar data by index
    // Parameters:
    // - index: The index of the bar
    // Returns: The bar data if found
    virtual std::optional<BarData> GetBarByIndex(int index) const = 0;

    // Retrieves all trade statistics
    // Returns: The trade statistics
    virtual TradeStatistics GetAccountWideTradeStatistics() const = 0;

    // Retrieves all trade statistics
    // Returns: The trade statistics
    virtual TradeStatistics GetCurrentSessionTradeStatistics(bool flatToFlat = false) const = 0;

    // Retrieves position data
    // Returns: The position data
    virtual PositionData GetPositionData() const = 0;

    // Platform initialization
    // Initializes the trading platform
    // Parameters:
    // - tradeContext: The trading context
    // - startDate, endDate: Optional start and end dates
    virtual void InitializePlatform(ContextType tradeContext, std::optional<DateTime> startDate = std::nullopt, std::optional<DateTime> endDate = std::nullopt) = 0;

    // Platform update settings
    // Sets the bar period
    // Parameters:
    // - barType: The type of the bar
    // - barPeriod: The period of the bar
    virtual void SetBarPeriod(const std::string& barType, std::string barPeriod) = 0;

    // Sets the update interval type
    // Parameters:
    // - updateIntervalType: The type of update interval
    virtual void SetUpdateIntervalType(const UpdateIntervalType updateIntervalType) = 0;

    // Market Data Functions
    // Retrieves the bid-ask spread
    // Returns: The bid-ask spread as a pair
    virtual std::pair<double, double> GetBidAskSpread() const = 0;

    // Market Depth Functions
    // Retrieves the bid market depth
    // Parameters:
    // - depthLevel: The depth level to retrieve
    // Returns: The bid market depth data
    virtual std::vector<MarketDepth> GetBidMarketDepth(int depthLevel = 0) const = 0;

    // Retrieves the ask market depth
    // Parameters:
    // - depthLevel: The depth level to retrieve
    // Returns: The ask market depth data
    virtual std::vector<MarketDepth> GetAskMarketDepth(int depthLevel = 0) const = 0;

    // Market By Order (MBO) Functions
    // Retrieves bid market orders for a specific price
    // Parameters:
    // - price: The price level to retrieve orders for
    // Returns: The market order data
    virtual std::vector<MarketOrderData> GetBidMarketOrdersForPrice(double price) const = 0;

    // Retrieves ask market orders for a specific price
    // Parameters:
    // - price: The price level to retrieve orders for
    // Returns: The market order data
    virtual std::vector<MarketOrderData> GetAskMarketOrdersForPrice(double price) const = 0;

    // Time and Sales Functions
    // Retrieves time and sales data
    // Returns: The time and sales data
    virtual std::vector<TimeAndSales> GetTimeAndSalesData() = 0;

    // Retrieves the latest time and sales data
    // Returns: The latest time and sales data if available
    virtual std::optional<TimeAndSales> GetLatestTimeAndSales() = 0;

    // Retrieves time and sales data for a specific bar index
    // Parameters:
    // - barIndex: The bar index to retrieve data for
    // Returns: The time and sales data
    virtual std::vector<TimeAndSales> GetTimeAndSalesForBarIndex(int barIndex) = 0;

    // Retrieves the current price
    // Returns: The current price
    virtual double GetCurrentPrice() = 0;

    // Retrieves the tick size
    // Returns: The tick size
    virtual double GetTickSize() = 0;

    // Retrieves the currency value per tick
    // Returns: The currency value per tick
    virtual double GetCurrencyValuePerTick() = 0;

    // Checks if the platform is ready for the next trade iteration
    // Returns: True if ready, otherwise false
    virtual bool IsReadyForTradeIteration() = 0;

    // Retrieves the platform context
    // Returns: The platform context
    virtual ContextType GetPlatformContext() const = 0;

    virtual DateTime GetCurrentPlatformTime(bool getBasedOnBar = true) const = 0;
};
