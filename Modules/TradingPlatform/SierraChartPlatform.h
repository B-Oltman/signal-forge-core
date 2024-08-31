#ifndef SIERRA_CHART_PLATFORM_H
#define SIERRA_CHART_PLATFORM_H

#include "ITradingPlatform.h"
#include "SierraChartHelpers.h"
#include "CommonTypes.h"
#include <sierrachart.h>
#include <string>
#include <vector>
#include <optional>
#include <iostream>
#include <shared_mutex>
#include <algorithm>

class SierraChartPlatform : public ITradingPlatform
{
public:
    virtual ~SierraChartPlatform() = default;

    SierraChartPlatform(s_sc *sc, ContextType context) : sc(sc), tradePlatformContext_(context) {}

    double GetCurrentPrice() override
    {
        std::shared_lock<std::shared_mutex> lock(scMutex);
        return sc->Close[sc->Index];
    }

    // Message Functions
    void AddMessageToLog(const std::string &message, bool showLog) override
    {
        std::unique_lock<std::shared_mutex> lock(scMutex);
        sc->AddMessageToLog(message.c_str(), showLog);
    }

    // Trade Functions
    std::optional<ExecutedOrder> BuyEntry(PendingOrder pendingOrder) override
    {
        std::unique_lock<std::shared_mutex> lock(scMutex);
        s_SCNewOrder newOrder;
        newOrder.OrderType = SierraChartHelpers::MapOrderTypeToSCTOrderType(pendingOrder.orderType);
        newOrder.OrderQuantity = pendingOrder.quantity;
        newOrder.Price1 = pendingOrder.price;

        double filledQuantity = sc->BuyOrder(newOrder);

        if (filledQuantity > 0)
        {
            auto activeOrder = SierraChartHelpers::GetOrderDetails(sc, newOrder.InternalOrderID);

            if (activeOrder.has_value())
            {
                if (filledQuantity == pendingOrder.quantity)
                {
                    activeOrder->status = OrderStatus::Filled;
                    return activeOrder;
                }
                else
                {
                    activeOrder->status = OrderStatus::PartiallyFilled;
                    return activeOrder;
                }

                activeOrder->direction = OrderDirection::Buy;

                return activeOrder;
            }
        }

        return std::nullopt;
    }

    std::optional<ExecutedOrder> SellEntry(PendingOrder pendingOrder) override
    {
        std::unique_lock<std::shared_mutex> lock(scMutex);
        s_SCNewOrder newOrder;
        newOrder.OrderType = SierraChartHelpers::MapOrderTypeToSCTOrderType(pendingOrder.orderType);
        newOrder.OrderQuantity = pendingOrder.quantity;
        newOrder.Price1 = pendingOrder.price;

        double filledQuantity = sc->SellOrder(newOrder);

        if (filledQuantity > 0)
        {
            auto activeOrder = SierraChartHelpers::GetOrderDetails(sc, newOrder.InternalOrderID);

            if (activeOrder.has_value())
            {
                if (filledQuantity == pendingOrder.quantity)
                {
                    activeOrder->status = OrderStatus::Filled;
                    return activeOrder;
                }
                else
                {
                    activeOrder->status = OrderStatus::PartiallyFilled;
                    return activeOrder;
                }

                return activeOrder;
            }
        }

        return std::nullopt;
    }

    std::optional<ExecutedOrder> BuyExit(ExecutedOrder activeOrder) override
    {
        std::unique_lock<std::shared_mutex> lock(scMutex);
        s_SCNewOrder exitOrder;
        exitOrder.InternalOrderID = activeOrder.orderId;

        double filledQuantity = sc->BuyExit(exitOrder);

        if (filledQuantity > 0)
        {
            if (filledQuantity == activeOrder.requestedQuantity)
            {
                activeOrder.exitTime = DateTime();
                activeOrder.status = OrderStatus::Filled;
                return activeOrder;
            }
            else
            {
                activeOrder.exitTime = DateTime();
                activeOrder.status = OrderStatus::PartiallyFilled;
                return activeOrder;
            }

            return activeOrder;
        }

        return std::nullopt;
    }

    std::optional<ExecutedOrder> SellExit(ExecutedOrder activeOrder) override
    {
        std::unique_lock<std::shared_mutex> lock(scMutex);
        s_SCNewOrder exitOrder;
        exitOrder.InternalOrderID = activeOrder.orderId;

        double filledQuantity = sc->SellExit(exitOrder);

        if (filledQuantity > 0)
        {
            if (filledQuantity == activeOrder.requestedQuantity)
            {
                activeOrder.exitTime = DateTime();
                activeOrder.status = OrderStatus::Filled;
                return activeOrder;
            }
            else
            {
                activeOrder.exitTime = DateTime();
                activeOrder.status = OrderStatus::PartiallyFilled;
                return activeOrder;
            }

            return activeOrder;
        }

        return std::nullopt;
    }

    std::optional<ExecutedOrder> CloseOrCancelOrder(ExecutedOrder activeOrder) override
    {
        std::unique_lock<std::shared_mutex> lock(scMutex);
        int orderID = activeOrder.orderId;
        if (orderID > 0)
        {
            s_SCNewOrder cancelOrder;
            cancelOrder.InternalOrderID = orderID;
            int cancelResult = sc->CancelOrder(orderID);

            if (cancelResult == SCTRADING_ORDER_ERROR)
            {
                sc->AddMessageToLog("Order cancellation failed.", 1);
            }
            else
            {
                sc->AddMessageToLog("Order successfully canceled.", 1);
            }

            std::optional<ExecutedOrder> cancelledOrder = std::nullopt;
            if (activeOrder.direction == OrderDirection::Buy)
            {
                cancelledOrder = BuyExit(activeOrder);
                cancelledOrder->status = OrderStatus::Cancelled;
            }
            else
            {
                cancelledOrder = SellExit(activeOrder);
                cancelledOrder->status = OrderStatus::Cancelled;
            }

            return cancelledOrder;
        }

        return std::nullopt;
    }

    std::optional<std::vector<ExecutedOrder>> SubmitOCOOrder(PendingOrder pendingOrder, double price1, double price2, double target1Offset, double stop1Offset, double target2Offset = 0, double stop2Price = 0) override
    {
        std::unique_lock<std::shared_mutex> lock(scMutex);
        s_SCNewOrder ocoOrder;
        ocoOrder.OrderQuantity = pendingOrder.quantity;
        ocoOrder.OrderType = SCT_ORDERTYPE_OCO_BUY_STOP_SELL_STOP;
        ocoOrder.TimeInForce = SCT_TIF_GOOD_TILL_CANCELED;
        ocoOrder.Price1 = price1;
        ocoOrder.Price2 = price2;

        ocoOrder.AttachedOrderTarget1Type = SCT_ORDERTYPE_LIMIT;
        ocoOrder.Target1Offset = target1Offset;
        ocoOrder.AttachedOrderStop1Type = SCT_ORDERTYPE_STOP;
        ocoOrder.Stop1Offset = stop1Offset;

        ocoOrder.Target1Offset_2 = target2Offset;
        ocoOrder.Stop1Price_2 = stop2Price;

        double result = sc->SubmitOCOOrder(ocoOrder);

        if (result > 0)
        {
            std::vector<ExecutedOrder> executedOrders;

            if (ocoOrder.InternalOrderID > 0)
            {
                ExecutedOrder executedOrder1 = SierraChartHelpers::GetOrderDetails(sc, ocoOrder.InternalOrderID).value();
                executedOrders.push_back(executedOrder1);
            }
            else
            {
                AddMessageToLog("OCO Order 1 was rejected", true);
            }

            if (ocoOrder.InternalOrderID2 > 0)
            {
                ExecutedOrder executedOrder2 = SierraChartHelpers::GetOrderDetails(sc, ocoOrder.InternalOrderID2).value();
                executedOrders.push_back(executedOrder2);
            }
            else
            {
                AddMessageToLog("OCO Order 2 was rejected", true);
            }

            return executedOrders;
        }
        else
        {
            AddMessageToLog("SubmitOCOOrder failed with result: " + std::to_string(result), true);
        }

        return std::nullopt;
    }

    // Data Retrieval Functions
    std::optional<BarData> GetBarByOffsetFromCurrent(int offsetFromCurrentBar = 0) const override
    {
        std::shared_lock<std::shared_mutex> lock(scMutex);
        int index = sc->Index - offsetFromCurrentBar;
        if (index < 0 || index >= sc->ArraySize)
        {
            return std::nullopt;
        }

        return BarData(
            DateTime(sc->BaseDateTimeIn[index].ToUNIXTime()),
            DateTime(sc->BaseDataEndDateTime[index].ToUNIXTime()),
            sc->BaseData[SC_OPEN][index],
            sc->BaseData[SC_HIGH][index],
            sc->BaseData[SC_LOW][index],
            sc->BaseData[SC_CLOSE][index],
            sc->BaseData[SC_VOLUME][index],
            sc->NumberOfTrades[index],
            sc->OpenInterest[index],
            sc->OHLCAvg[index],
            sc->HLCAvg[index],
            sc->HLAvg[index],
            sc->BidVolume[index],
            sc->AskVolume[index],
            sc->UpTickVolume[index],
            sc->DownTickVolume[index],
            sc->NumberOfBidTrades[index],
            sc->NumberOfAskTrades[index]);
    }

    std::optional<BarData> GetBarByTime(const DateTime &dateTime) const override
    {
        std::shared_lock<std::shared_mutex> lock(scMutex);
        for (int index = 0; index < sc->ArraySize; ++index)
        {
            if (sc->BaseDataEndDateTime[index].GetAsDouble() == dateTime.ToTimeT())
            {
                return BarData(
                    DateTime(sc->BaseDateTimeIn[index].ToUNIXTime()),
                    DateTime(sc->BaseDataEndDateTime[index].ToUNIXTime()),
                    sc->BaseData[SC_OPEN][index],
                    sc->BaseData[SC_HIGH][index],
                    sc->BaseData[SC_LOW][index],
                    sc->BaseData[SC_CLOSE][index],
                    sc->BaseData[SC_VOLUME][index],
                    sc->NumberOfTrades[index],
                    sc->OpenInterest[index],
                    sc->OHLCAvg[index],
                    sc->HLCAvg[index],
                    sc->HLAvg[index],
                    sc->BidVolume[index],
                    sc->AskVolume[index],
                    sc->UpTickVolume[index],
                    sc->DownTickVolume[index],
                    sc->NumberOfBidTrades[index],
                    sc->NumberOfAskTrades[index]);
            }
        }
        return std::nullopt;
    }

    std::vector<BarData> GetBarRangeByOffset(int startOffset, int endOffset) const override
    {
        std::shared_lock<std::shared_mutex> lock(scMutex);
        std::vector<BarData> bars;
        int startIndex = sc->Index - startOffset;
        int endIndex = sc->Index - endOffset;

        if (startIndex < 0)
            startIndex = 0;
        if (endIndex >= sc->ArraySize)
            endIndex = sc->ArraySize - 1;

        for (int i = startIndex; i <= endIndex; ++i)
        {
            auto barData = GetBarByOffsetFromCurrent(sc->Index - i);
            if (barData)
            {
                bars.push_back(*barData);
            }
        }
        return bars;
    }

    std::vector<BarData> GetBarRangeByTime(const DateTime &startDateTime, const DateTime &endDateTime) const override
    {
        std::shared_lock<std::shared_mutex> lock(scMutex);
        std::vector<BarData> bars;
        DateTime currentTime = startDateTime;
        while (currentTime <= endDateTime)
        {
            auto barData = GetBarByTime(currentTime);
            if (barData)
            {
                bars.push_back(*barData);
            }
            currentTime += std::chrono::minutes(1); // Increment the time by the bar interval (e.g., 1 minute)
        }
        return bars;
    }

    std::optional<BarData> GetBarByIndex(int index) const override
    {
        std::shared_lock<std::shared_mutex> lock(scMutex);
        if (index < 0 || index >= sc->ArraySize)
        {
            return std::nullopt;
        }

        return BarData(
            DateTime(sc->BaseDateTimeIn[index].ToUNIXTime()),
            DateTime(sc->BaseDataEndDateTime[index].ToUNIXTime()),
            sc->BaseData[SC_OPEN][index],
            sc->BaseData[SC_HIGH][index],
            sc->BaseData[SC_LOW][index],
            sc->BaseData[SC_CLOSE][index],
            sc->BaseData[SC_VOLUME][index],
            sc->NumberOfTrades[index],
            sc->OpenInterest[index],
            sc->OHLCAvg[index],
            sc->HLCAvg[index],
            sc->HLAvg[index],
            sc->BidVolume[index],
            sc->AskVolume[index],
            sc->UpTickVolume[index],
            sc->DownTickVolume[index],
            sc->NumberOfBidTrades[index],
            sc->NumberOfAskTrades[index]);
    }

    TradeStatistics GetAccountWideTradeStatistics() const override
    {
        // TODO: Need better stat calculation, use GetTradeListEntry() and iterate through the trades of the chart to calculate.
        std::shared_lock<std::shared_mutex> lock(scMutex);
        n_ACSIL::s_TradeStatistics scStats;
        sc->GetTradeStatisticsForSymbolV2(n_ACSIL::TradeStatisticsTypeEnum::STATS_TYPE_ALL_TRADES, scStats);
        return SierraChartHelpers::MapToTradeStatistics(scStats);
    }

    TradeStatistics GetCurrentSessionTradeStatistics(bool flatToFlat = false) const override
    {
        int tradeListSize = sc->GetTradeListSize();
        int flatToFlatTradeListSize = sc->GetFlatToFlatTradeListSize();

        s_ACSTrade tradeEntry;

        // Initialize statistics
        auto stats = TradeStatistics();

        double totalProfit = 0.0;
        double totalProfitFlatToFlat = 0.0;
        double totalLoss = 0.0;
        double totalLossFlatToFlat = 0.0;
        double totalWinQuantity = 0.0;
        double totalLossQuantity = 0.0;
        double totalWinQuantityFlatToFlat = 0.0;
        double totalLossQuantityFlatToFlat = 0.0;

        if (!flatToFlat)
        {
            for (int i = 0; i < tradeListSize; i++)
            {
                if (sc->GetTradeListEntry(i, tradeEntry))
                {
                    // Update trade statistics
                    totalProfit += Max(0.0, tradeEntry.TradeProfitLoss);
                    totalLoss += Min(0.0, tradeEntry.TradeProfitLoss);
                    stats.totalTrades++;
                    stats.totalFilledQuantity += tradeEntry.TradeQuantity;
                    stats.totalCommission += tradeEntry.Commission;
                    stats.maximumOpenPositionProfit = Max(stats.maximumOpenPositionProfit, tradeEntry.MaximumOpenPositionProfit);
                    stats.maximumOpenPositionLoss = Max(stats.maximumOpenPositionLoss, tradeEntry.MaximumOpenPositionLoss);
                    stats.largestTradeQuantity = Max(stats.largestTradeQuantity, static_cast<double>(tradeEntry.TradeQuantity));

                    if (tradeEntry.TradeProfitLoss > 0)
                    {
                        stats.winningTrades++;
                        stats.averageWin += tradeEntry.TradeProfitLoss;
                        totalWinQuantity += tradeEntry.TradeQuantity;
                    }
                    else if (tradeEntry.TradeProfitLoss < 0)
                    {
                        stats.losingTrades++;
                        stats.averageLoss += tradeEntry.TradeProfitLoss;
                        totalLossQuantity += tradeEntry.TradeQuantity;
                    }

                    stats.lastFillDateTime = DateTime(tradeEntry.CloseDateTime.ToUNIXTime());
                    stats.lastEntryDateTime = DateTime(tradeEntry.OpenDateTime.ToUNIXTime());
                    stats.lastExitDateTime = DateTime(tradeEntry.CloseDateTime.ToUNIXTime());
                }
            }

            // Update averages
            if (stats.winningTrades > 0)
            {
                stats.averageWin /= stats.winningTrades;
                stats.totalWinningQuantity = totalWinQuantity;
            }
            if (stats.losingTrades > 0)
            {
                stats.averageLoss /= stats.losingTrades;
                stats.totalLosingQuantity = totalLossQuantity;
            }

            // Calculate additional statistics like Sharpe, Sortino ratios etc.
            stats.profit = totalProfit + totalLoss;
            stats.maxDrawdown = Min(stats.maxDrawdown, totalLoss); // Example, you'll want to properly calculate drawdown
            stats.winRate = (stats.totalTrades > 0) ? (static_cast<double>(stats.winningTrades) / stats.totalTrades) * 100.0 : 0.0;
        }
        else
        {
            // Repeat the same logic for FlatToFlat trades
            for (int i = 0; i < flatToFlatTradeListSize; i++)
            {
                if (sc->GetFlatToFlatTradeListEntry(i, tradeEntry))
                {
                    // Update flat-to-flat trade statistics similarly to regular trade stats
                    totalProfitFlatToFlat += Max(0.0, tradeEntry.TradeProfitLoss);
                    totalLossFlatToFlat += Min(0.0, tradeEntry.TradeProfitLoss);
                    stats.totalTrades++;
                    stats.totalFilledQuantity += tradeEntry.TradeQuantity;
                    stats.totalCommission += tradeEntry.Commission;
                    stats.maximumOpenPositionProfit = Max(stats.maximumOpenPositionProfit, tradeEntry.MaximumOpenPositionProfit);
                    stats.maximumOpenPositionLoss = Max(stats.maximumOpenPositionLoss, tradeEntry.MaximumOpenPositionLoss);
                    stats.largestTradeQuantity = Max(stats.largestTradeQuantity, static_cast<double>(tradeEntry.TradeQuantity));

                    if (tradeEntry.TradeProfitLoss > 0)
                    {
                        stats.winningTrades++;
                        stats.averageWin += tradeEntry.TradeProfitLoss;
                        totalWinQuantityFlatToFlat += tradeEntry.TradeQuantity;
                    }
                    else if (tradeEntry.TradeProfitLoss < 0)
                    {
                        stats.losingTrades++;
                        stats.averageLoss += tradeEntry.TradeProfitLoss;
                        totalLossQuantityFlatToFlat += tradeEntry.TradeQuantity;
                    }

                    stats.lastFillDateTime = DateTime(tradeEntry.CloseDateTime.ToUNIXTime());
                    stats.lastEntryDateTime = DateTime(tradeEntry.OpenDateTime.ToUNIXTime());
                    stats.lastExitDateTime = DateTime(tradeEntry.CloseDateTime.ToUNIXTime());
                }
            }

            // Update averages for flat-to-flat statistics
            if (stats.winningTrades > 0)
            {
                stats.averageWin /= stats.winningTrades;
                stats.totalWinningQuantity = totalWinQuantityFlatToFlat;
            }
            if (stats.losingTrades > 0)
            {
                stats.averageLoss /= stats.losingTrades;
                stats.totalLosingQuantity = totalLossQuantityFlatToFlat;
            }

            stats.profit = totalProfitFlatToFlat + totalLossFlatToFlat;
            stats.maxDrawdown = Min(stats.maxDrawdown, totalLossFlatToFlat); // Example, you'll want to properly calculate drawdown
            stats.winRate = (stats.totalTrades > 0) ? (static_cast<double>(stats.winningTrades) / stats.totalTrades) * 100.0 : 0.0;
        }

        return stats;
    }

    void InitializePlatform(ContextType tradeContext, std::optional<DateTime> startDate, std::optional<DateTime> endDate) override
    {
        std::unique_lock<std::shared_mutex> lock(scMutex);
        if (tradeContext == ContextType::Backtesting)
        {
            sc->DisconnectFromExternalServiceServer = true;

            if (startDate.has_value() && endDate.has_value())
            {
                sc->IsFullRecalculation = true;
                sc->LoadChartDataByDateRange = true;
                sc->ChartDataType = INTRADAY_DATA;
            }

            sc->SendOrdersToTradeService = false;
            if (startDate.has_value())
            {
                sc->ChartDataStartDate = SCDateTime(startDate.value().timePoint).GetDate();
            }

            if (endDate.has_value())
            {
                sc->ChartDataEndDate = SCDateTime(endDate.value().timePoint).GetDate();
            }
        }
        else if (tradeContext == ContextType::LiveTrading)
        {
            sc->ConnectToExternalServiceServer = true;
            sc->SendOrdersToTradeService = true;
        }
        else
        {
            sc->ConnectToExternalServiceServer = false;
            sc->SendOrdersToTradeService = false;
        }

        sc->AllowOppositeEntryWithOpposingPositionOrOrders = false;
        sc->CancelAllOrdersOnEntriesAndReversals = false;
        sc->CancelAllWorkingOrdersOnExit = true;
        sc->AllowOnlyOneTradePerBar = false;
        sc->UsesMarketDepthData = 1;
        sc->MaintainAdditionalChartDataArrays = 1;

        // TODO: Need to determine how I can use sc to load only bars within the backtest start and end date.
    }

    bool IsReadyForTradeIteration() override
    {
        std::shared_lock<std::shared_mutex> lock(scMutex);
        if (sc->IsFullRecalculation)
        {
            if (sc->Index == 0)
            {
                return true;
            }
            return false;
        }

        if (updateIntervalType_ == UpdateIntervalType::New_Bar)
        {
            if (sc->Index == lastBarIndexProcessed)
            {
                return false;
            }
            lastBarIndexProcessed = sc->Index;
        }
        else
        {
            if (sc->Close[sc->Index] == lastClosePrice)
            {
                return false;
            }
            lastClosePrice = sc->Close[sc->Index];
        }

        return true;
    }

    PositionData GetPositionData() const override
    {
        std::shared_lock<std::shared_mutex> lock(scMutex);
        s_SCPositionData PositionData;
        sc->GetTradePosition(PositionData);
        return SierraChartHelpers::MapToPositionData(PositionData);
    }

    // Platform update settings
void SetBarPeriod(const std::string &barType, std::string barPeriod) override
{
    std::unique_lock<std::shared_mutex> lock(scMutex);
    n_ACSIL::s_BarPeriod newBarPeriod;
    newBarPeriod.ChartDataType = INTRADAY_DATA;

    // Split the barPeriod string by dash ('-') delimiter
    std::vector<int> params;
    std::istringstream iss(barPeriod);
    std::string param;
    while (std::getline(iss, param, '-'))
    {
        params.push_back(std::stoi(param));
    }

    if (barType == "s" || barType == "m" || barType == "h" || barType == "t" || barType == "v" ||
        barType == "r" || barType == "rm" || barType == "rt" || barType == "rf" || barType == "ro" ||
        barType == "rmo" || barType == "rk" || barType == "ar" || barType == "dv" || barType == "pc" ||
        barType == "d" || barType == "w" || barType == "mn" || barType == "q" || barType == "y")
    {
        // These bar types expect 1 parameter
        newBarPeriod.IntradayChartBarPeriodParameter1 = params[0];
        
        if (barType == "m") {
            newBarPeriod.IntradayChartBarPeriodParameter1 *= 60; // Convert minutes to seconds
        } else if (barType == "h") {
            newBarPeriod.IntradayChartBarPeriodParameter1 *= 3600; // Convert hours to seconds
        }
    }
    else if (barType == "rv" || barType == "pf" || barType == "pnf")
    {
        // These bar types expect 2 parameters
        newBarPeriod.IntradayChartBarPeriodParameter1 = params[0];
        if (params.size() > 1)
            newBarPeriod.IntradayChartBarPeriodParameter2 = params[1];
    }
    else if (barType == "fr" || barType == "fri")
    {
        // These bar types expect 3 parameters
        newBarPeriod.IntradayChartBarPeriodParameter1 = params[0];
        if (params.size() > 1)
            newBarPeriod.IntradayChartBarPeriodParameter2 = params[1];
        if (params.size() > 2)
            newBarPeriod.IntradayChartBarPeriodParameter3 = params[2];
    }

    // Set the appropriate IntradayChartBarPeriodType or HistoricalChartBarPeriodType based on the barType
    if (barType == "s" || barType == "m" || barType == "h") {
        newBarPeriod.IntradayChartBarPeriodType = IBPT_DAYS_MINS_SECS;
    } else if (barType == "t") {
        newBarPeriod.IntradayChartBarPeriodType = IBPT_NUM_TRADES_PER_BAR;
    } else if (barType == "v") {
        newBarPeriod.IntradayChartBarPeriodType = IBPT_VOLUME_PER_BAR;
    } else if (barType == "r") {
        newBarPeriod.IntradayChartBarPeriodType = IBPT_RANGE_IN_TICKS_STANDARD;
    } else if (barType == "rm") {
        newBarPeriod.IntradayChartBarPeriodType = IBPT_RANGE_IN_TICKS_NEWBAR_ON_RANGEMET;
    } else if (barType == "rt") {
        newBarPeriod.IntradayChartBarPeriodType = IBPT_RANGE_IN_TICKS_TRUE;
    } else if (barType == "rf") {
        newBarPeriod.IntradayChartBarPeriodType = IBPT_RANGE_IN_TICKS_FILL_GAPS;
    } else if (barType == "ro") {
        newBarPeriod.IntradayChartBarPeriodType = IBPT_RANGE_IN_TICKS_OPEN_EQUAL_CLOSE;
    } else if (barType == "rmo") {
        newBarPeriod.IntradayChartBarPeriodType = IBPT_RANGE_IN_TICKS_NEW_BAR_ON_RANGE_MET_OPEN_EQUALS_PRIOR_CLOSE;
    } else if (barType == "rv") {
        newBarPeriod.IntradayChartBarPeriodType = IBPT_REVERSAL_IN_TICKS;
    } else if (barType == "rk") {
        newBarPeriod.IntradayChartBarPeriodType = IBPT_RENKO_IN_TICKS;
    } else if (barType == "fr") {
        newBarPeriod.IntradayChartBarPeriodType = IBPT_FLEX_RENKO_IN_TICKS;
    } else if (barType == "ar") {
        newBarPeriod.IntradayChartBarPeriodType = IBPT_ALIGNED_RENKO;
    } else if (barType == "dv") {
        newBarPeriod.IntradayChartBarPeriodType = IBPT_DELTA_VOLUME_PER_BAR;
    } else if (barType == "pc") {
        newBarPeriod.IntradayChartBarPeriodType = IBPT_PRICE_CHANGES_PER_BAR;
    } else if (barType == "d") {
        newBarPeriod.HistoricalChartBarPeriodType = HISTORICAL_CHART_PERIOD_DAYS;
    } else if (barType == "w") {
        newBarPeriod.HistoricalChartBarPeriodType = HISTORICAL_CHART_PERIOD_WEEKLY;
    } else if (barType == "mn") {
        newBarPeriod.HistoricalChartBarPeriodType = HISTORICAL_CHART_PERIOD_MONTHLY;
    } else if (barType == "q") {
        newBarPeriod.HistoricalChartBarPeriodType = HISTORICAL_CHART_PERIOD_QUARTERLY;
    } else if (barType == "y") {
        newBarPeriod.HistoricalChartBarPeriodType = HISTORICAL_CHART_PERIOD_YEARLY;
    } else if (barType == "fri") {
        newBarPeriod.IntradayChartBarPeriodType = IBPT_FLEX_RENKO_IN_TICKS_INVERSE_SETTINGS;
    } else if (barType == "pf" || barType == "pnf") {
        newBarPeriod.IntradayChartBarPeriodType = IBPT_POINT_AND_FIGURE;
    }

    sc->SetBarPeriodParameters(newBarPeriod);
}


    double GetTickSize() override
    {
        return sc->TickSize;
    }

    double GetCurrencyValuePerTick() override
    {
        return sc->CurrencyValuePerTick;
    }

    ContextType GetPlatformContext() const override
    {
        std::shared_lock<std::shared_mutex> lock(scMutex);
        return tradePlatformContext_;
    }

    void SetUpdateIntervalType(UpdateIntervalType updateIntervalType) override
    {
        std::unique_lock<std::shared_mutex> lock(scMutex);
        updateIntervalType_ = updateIntervalType;
        if (updateIntervalType == UpdateIntervalType::New_Bar)
        {
            sc->UpdateAlways = 0;
        }
        else
        {
            sc->UpdateAlways = 1;
        }
    }

    std::vector<TimeAndSales> GetTimeAndSalesData() override
    {
        std::shared_lock<std::shared_mutex> lock(scMutex);
        c_SCTimeAndSalesArray tsArray;
        sc->GetTimeAndSales(tsArray);

        std::vector<TimeAndSales> result;
        for (int i = 0; i < tsArray.Size(); ++i)
        {
            result.push_back(SierraChartHelpers::MapToTimeAndSales(tsArray[i], sc));
        }
        return result;
    }

    std::optional<TimeAndSales> GetLatestTimeAndSales() override
    {
        std::shared_lock<std::shared_mutex> lock(scMutex);
        c_SCTimeAndSalesArray tsArray;
        sc->GetTimeAndSales(tsArray);

        if (tsArray.Size() > 0)
        {
            return SierraChartHelpers::MapToTimeAndSales(tsArray[tsArray.Size() - 1], sc);
        }
        return std::nullopt;
    }

    std::vector<TimeAndSales> GetTimeAndSalesForBarIndex(int barIndex) override
    {
        std::shared_lock<std::shared_mutex> lock(scMutex);
        int beginIndex, endIndex;
        sc->GetTimeSalesArrayIndexesForBarIndex(barIndex, beginIndex, endIndex);

        std::vector<TimeAndSales> result;
        c_SCTimeAndSalesArray tsArray;
        sc->GetTimeAndSales(tsArray);

        if (beginIndex >= 0 && endIndex >= 0)
        {
            for (int i = beginIndex; i <= endIndex; ++i)
            {
                result.push_back(SierraChartHelpers::MapToTimeAndSales(tsArray[i], sc));
            }
        }
        return result;
    }

    // Market Depth Functions
    std::vector<MarketDepth> GetBidMarketDepth(int depthLevel) const override
    {
        std::shared_lock<std::shared_mutex> lock(scMutex);
        std::vector<MarketDepth> bidDepth;

        int bidLevels = depthLevel > 0 ? depthLevel : sc->GetBidMarketDepthNumberOfLevels();
        for (int level = 0; level < bidLevels; ++level)
        {
            s_MarketDepthEntry depthEntry;
            if (sc->GetBidMarketDepthEntryAtLevel(depthEntry, level))
            {
                MarketDepth md;
                md.timestamp = GetCurrentPlatformTime(); // Use current time or fetch from depthEntry if available
                md.type = MarketDepthType::Bid;
                md.price = depthEntry.AdjustedPrice;
                md.quantity = depthEntry.Quantity;
                md.numOrders = depthEntry.NumOrders;
                md.stackPullValue = sc->GetBidMarketDepthStackPullValueAtPrice(depthEntry.AdjustedPrice);

                bidDepth.push_back(md);
            }
        }

        return bidDepth;
    }

    std::vector<MarketDepth> GetAskMarketDepth(int depthLevel) const override
    {
        std::shared_lock<std::shared_mutex> lock(scMutex);
        std::vector<MarketDepth> askDepth;

        int askLevels = depthLevel > 0 ? depthLevel : sc->GetAskMarketDepthNumberOfLevels();
        for (int level = 0; level < askLevels; ++level)
        {
            s_MarketDepthEntry depthEntry;
            if (sc->GetAskMarketDepthEntryAtLevel(depthEntry, level))
            {
                MarketDepth md;
                md.timestamp = GetCurrentPlatformTime(); // Use current time or fetch from depthEntry if available
                md.type = MarketDepthType::Ask;
                md.price = depthEntry.AdjustedPrice;
                md.quantity = depthEntry.Quantity;
                md.numOrders = depthEntry.NumOrders;
                md.stackPullValue = sc->GetAskMarketDepthStackPullValueAtPrice(depthEntry.AdjustedPrice);

                askDepth.push_back(md);
            }
        }

        return askDepth;
    }

    // Market By Order (MBO) Functions
    std::vector<MarketOrderData> GetBidMarketOrdersForPrice(double price) const override
    {
        std::shared_lock<std::shared_mutex> lock(scMutex);
        std::vector<MarketOrderData> marketOrders;
        int priceInTicks = static_cast<int>(price / sc->TickSize);
        n_ACSIL::s_MarketOrderData marketOrderDataArray[100]; // Example array size

        int numOrders = sc->GetBidMarketLimitOrdersForPrice(priceInTicks, 100, marketOrderDataArray);

        for (int i = 0; i < numOrders; ++i)
        {
            MarketOrderData mod;
            mod.timestamp = GetCurrentPlatformTime(); // Use current time or fetch from data source if available
            mod.price = price;
            mod.quantity = marketOrderDataArray[i].OrderQuantity;
            mod.orderId = marketOrderDataArray[i].OrderID;
            marketOrders.push_back(mod);
        }

        return marketOrders;
    }

    std::vector<MarketOrderData> GetAskMarketOrdersForPrice(double price) const override
    {
        std::shared_lock<std::shared_mutex> lock(scMutex);
        std::vector<MarketOrderData> marketOrders;
        int priceInTicks = static_cast<int>(price / sc->TickSize);
        n_ACSIL::s_MarketOrderData marketOrderDataArray[100]; // Example array size

        int numOrders = sc->GetAskMarketLimitOrdersForPrice(priceInTicks, 100, marketOrderDataArray);

        for (int i = 0; i < numOrders; ++i)
        {
            MarketOrderData mod;
            mod.timestamp = GetCurrentPlatformTime(); // Use current time or fetch from data source if available
            mod.price = price;
            mod.quantity = marketOrderDataArray[i].OrderQuantity;
            mod.orderId = marketOrderDataArray[i].OrderID;
            marketOrders.push_back(mod);
        }

        return marketOrders;
    }

    std::pair<double, double> GetBidAskSpread() const override
    {
        std::shared_lock<std::shared_mutex> lock(scMutex);
        double bid = sc->Bid;
        double ask = sc->Ask;
        return {bid, ask};
    }

    DateTime GetCurrentPlatformTime(bool getBasedOnBar = true) const override
    {
        if(getBasedOnBar)
        {
            // Returns the current time of the last bar.
            return DateTime(sc->BaseDataEndDateTime[sc->Index].ToUNIXTime());
            
        } else {
            return DateTime(sc->GetCurrentDateTime().ToUNIXTime());
        }                             
    }

    void UpdateSCRef(s_sc *new_sc)
    {
        std::unique_lock<std::shared_mutex> lock(scMutex);
        sc = new_sc; // Update the reference directly
    }

private:
    mutable std::shared_mutex scMutex; // Ensure thread-safety with shared_mutex
    double lastClosePrice = 0;
    int lastBarIndexProcessed = -1;
    ContextType tradePlatformContext_;
    UpdateIntervalType updateIntervalType_;
    s_sc *sc;
};

#endif // SIERRA_CHART_PLATFORM_H
