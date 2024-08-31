#ifndef SIERRACHARTHELPERS_H
#define SIERRACHARTHELPERS_H

#include <sierrachart.h>
#include <vector>
#include <optional>
#include <chrono>
#include "CommonTypes.h"

class SierraChartHelpers {
public:
    static int ExecuteTrade(s_sc* sc, const TradeSignal& signal);
    static std::optional<ExecutedOrder> GetOrderDetails(s_sc* sc, int orderID);
    static void CancelOrder(s_sc* sc, int orderID);
    static void UpdateOrder(s_sc* sc, int orderID, const ExecutedOrder& updatedOrder);
    static void AddOpenOrder(std::vector<int>& openOrders, int orderID);
    static void RemoveOpenOrder(std::vector<int>& openOrders, int orderID);
    static std::vector<int> GetOpenOrders(const std::vector<int>& openOrders);
    static void SaveOpenOrders(const std::vector<int>& openOrders);
    static std::vector<int> LoadOpenOrders();
    static int MapOrderTypeToSCTOrderType(OrderType orderType);
    static std::optional<std::pair<ParameterType, ParameterValue>> DetermineInputType(const s_SCInput_145& scSetting);
    static std::optional<TradeSystemParameter> MapSCInputToPlatformParameter(const s_SCInput_145& scSetting);
    static int GetMaxInputs();
    static DateTime ConvertToDateTime(const SCDateTime& scDateTime);
    static TradeStatistics MapToTradeStatistics(const n_ACSIL::s_TradeStatistics& scStats);
    static PositionData MapToPositionData(const s_SCPositionData& scPositionData);
    static TimeAndSales MapToTimeAndSales(const s_TimeAndSales& tsData, s_sc* sc);
};

// Implementations of the static methods

inline int SierraChartHelpers::ExecuteTrade(s_sc* sc, const TradeSignal& signal) {
    s_SCNewOrder newOrder;
    // newOrder.OrderType = MapOrderTypeToSCTOrderType(signal.Type);
    // newOrder.OrderQuantity = signal.Quantity;
    // newOrder.Price1 = signal.Price;

    // int filledQuantity = 0;
    // if (signal.BuySignal) {
    //     filledQuantity = sc->BuyOrder(newOrder);
    // } else if (signal.SellSignal) {
    //     filledQuantity = sc->SellOrder(newOrder);
    // }

    // if (filledQuantity > 0) {
    //     AddOpenOrder(newOrder.InternalOrderID);
    // }

    // TODO: Find out how to actually get the order ID and return it
    return 0;
}

inline std::optional<ExecutedOrder> SierraChartHelpers::GetOrderDetails(s_sc* sc, int orderID) {
    s_SCTradeOrder order;
    if (sc->GetOrderByOrderID(orderID, order)) {
        ExecutedOrder activeOrder;
        activeOrder.orderId = order.InternalOrderID;
        activeOrder.direction = (order.BuySell == BSE_BUY) ? OrderDirection::Buy : OrderDirection::Sell;
        activeOrder.entryPrice = order.Price1;
        activeOrder.filledQuantity = order.OrderQuantity;        
        activeOrder.entryTime = DateTime(order.EntryDateTime.ToUNIXTime());

        return activeOrder;
    }
    return std::nullopt;
}

inline void SierraChartHelpers::CancelOrder(s_sc* sc, int orderID) {
    sc->CancelOrder(orderID); // TODO: finish and remove from pending orders.    
}

inline void SierraChartHelpers::UpdateOrder(s_sc* sc, int orderID, const ExecutedOrder& updatedOrder) {
    s_SCTradeOrder order;
    if (sc->GetOrderByOrderID(orderID, order)) {
        order.Price1 = updatedOrder.entryPrice;
    }
}

inline void SierraChartHelpers::AddOpenOrder(std::vector<int>& openOrders, int orderID) {
    openOrders.push_back(orderID);
    SaveOpenOrders(openOrders);
}

inline void SierraChartHelpers::RemoveOpenOrder(std::vector<int>& openOrders, int orderID) {
    openOrders.erase(std::remove(openOrders.begin(), openOrders.end(), orderID), openOrders.end());
    SaveOpenOrders(openOrders);
}

inline std::vector<int> SierraChartHelpers::GetOpenOrders(const std::vector<int>& openOrders) {
    return openOrders;
}

inline void SierraChartHelpers::SaveOpenOrders(const std::vector<int>& openOrders) {
    // Implement saving logic, e.g., to a file or persistent storage
}

inline std::vector<int> SierraChartHelpers::LoadOpenOrders() {
    // Implement loading logic, e.g., from a file or persistent storage
    return std::vector<int>{};
}

inline int SierraChartHelpers::MapOrderTypeToSCTOrderType(OrderType orderType) {
    switch (orderType) {
        case OrderType::Market: return SCT_ORDERTYPE_MARKET;
        case OrderType::Limit: return SCT_ORDERTYPE_LIMIT;
        case OrderType::Stop: return SCT_ORDERTYPE_STOP;
        case OrderType::StopLimit: return SCT_ORDERTYPE_STOP_LIMIT;
        case OrderType::MarketIfTouched: return SCT_ORDERTYPE_MARKET_IF_TOUCHED;
        case OrderType::LimitChase: return SCT_ORDERTYPE_LIMIT_CHASE;
        case OrderType::LimitTouchChase: return SCT_ORDERTYPE_LIMIT_TOUCH_CHASE;
        case OrderType::TrailingStop: return SCT_ORDERTYPE_TRAILING_STOP;
        case OrderType::TrailingStopLimit: return SCT_ORDERTYPE_TRAILING_STOP_LIMIT;
        case OrderType::TriggeredTrailingStop3Offsets: return SCT_ORDERTYPE_TRIGGERED_TRAILING_STOP_3_OFFSETS;
        case OrderType::TriggeredTrailingStopLimit3Offsets: return SCT_ORDERTYPE_TRIGGERED_TRAILING_STOP_LIMIT_3_OFFSETS;
        case OrderType::StepTrailingStop: return SCT_ORDERTYPE_STEP_TRAILING_STOP;
        case OrderType::StepTrailingStopLimit: return SCT_ORDERTYPE_STEP_TRAILING_STOP_LIMIT;
        case OrderType::TriggeredStepTrailingStop: return SCT_ORDERTYPE_TRIGGERED_STEP_TRAILING_STOP;
        case OrderType::TriggeredStepTrailingStopLimit: return SCT_ORDERTYPE_TRIGGERED_STEP_TRAILING_STOP_LIMIT;
        case OrderType::OcoLimitStop: return SCT_ORDERTYPE_OCO_LIMIT_STOP;
        case OrderType::OcoLimitStopLimit: return SCT_ORDERTYPE_OCO_LIMIT_STOP_LIMIT;
        case OrderType::OcoBuyStopSellStop: return SCT_ORDERTYPE_OCO_BUY_STOP_SELL_STOP;
        case OrderType::OcoBuyStopLimitSellStopLimit: return SCT_ORDERTYPE_OCO_BUY_STOP_LIMIT_SELL_STOP_LIMIT;
        case OrderType::OcoBuyLimitSellLimit: return SCT_ORDERTYPE_OCO_BUY_LIMIT_SELL_LIMIT;
        case OrderType::LimitIfTouched: return SCT_ORDERTYPE_LIMIT_IF_TOUCHED;
        case OrderType::BidAskQuantityTriggeredStop: return SCT_ORDERTYPE_BID_ASK_QUANTITY_TRIGGERED_STOP;
        case OrderType::TriggeredLimit: return SCT_ORDERTYPE_TRIGGERED_LIMIT;
        case OrderType::TradeVolumeTriggeredStop: return SCT_ORDERTYPE_TRADE_VOLUME_TRIGGERED_STOP;
        case OrderType::StopWithBidAskTriggering: return SCT_ORDERTYPE_STOP_WITH_BID_ASK_TRIGGERING;
        case OrderType::StopWithLastTriggering: return SCT_ORDERTYPE_STOP_WITH_LAST_TRIGGERING;
        case OrderType::LimitIfTouchedClientSide: return SCT_ORDERTYPE_LIMIT_IF_TOUCHED_CLIENT_SIDE;
        case OrderType::MarketIfTouchedClientSide: return SCT_ORDERTYPE_MARKET_IF_TOUCHED_CLIENT_SIDE;
        case OrderType::TradeVolumeTriggeredStopLimit: return SCT_ORDERTYPE_TRADE_VOLUME_TRIGGERED_STOP_LIMIT;
        case OrderType::StopLimitClientSide: return SCT_ORDERTYPE_STOP_LIMIT_CLIENT_SIDE;
        case OrderType::TriggeredStop: return SCT_ORDERTYPE_TRIGGERED_STOP;
        case OrderType::TriggeredLimitWithChase: return SCT_ORDERTYPE_TRIGGERED_LIMIT_WITH_CHASE;
        case OrderType::MarketLimit: return SCT_ORDERTYPE_MARKET_LIMIT;
        default: return -1; // Handle unknown order type
    }
}

inline std::optional<TradeSystemParameter> SierraChartHelpers::MapSCInputToPlatformParameter(const s_SCInput_145& scSetting) {
    TradeSystemParameter parameter;
    parameter.name = scSetting.Name;    

    switch (scSetting.ValueType) {
        case FLOAT_VALUE:
            parameter.valueType = ParameterType::Float;
            parameter.value = scSetting.FloatValue;
            parameter.minValue = scSetting.FloatValueMin;
            parameter.maxValue = scSetting.FloatValueMax;
            break;
        case INT_VALUE:
            parameter.valueType = ParameterType::Integer;
            parameter.value = scSetting.IntValue;
            parameter.minValue = scSetting.IntValueMin;
            parameter.maxValue = scSetting.IntValueMax;
            break;
        case YESNO_VALUE:
            parameter.valueType = ParameterType::Boolean;
            parameter.value = static_cast<bool>(scSetting.BooleanValue);
            break;
        case DOUBLE_VALUE:
            parameter.valueType = ParameterType::Double;
            parameter.value = scSetting.DoubleValue;
            parameter.minValue = scSetting.DoubleValueMin;
            parameter.maxValue = scSetting.DoubleValueMax;
            break;
        case STRING_VALUE:
            parameter.valueType = ParameterType::String;
            parameter.value = std::string(scSetting.GetString());
            break;
        case DATE_VALUE:
        case TIME_VALUE:
        case DATETIME_VALUE:
            parameter.valueType = ParameterType::DateTime;
            parameter.value = scSetting.DateTimeValue;
            parameter.minValue = scSetting.DateTimeValueMin;            
            parameter.maxValue = scSetting.DateTimeValueMax;
            break;
        case CUSTOM_STRING_VALUE:
            parameter.valueType = ParameterType::CustomString;
            parameter.value = scSetting.GetIndex();
            if (scSetting.InternalSetCustomStrings != nullptr) {
                // Extract the custom strings if possible
                std::string customStrings(scSetting.SelectedCustomInputString);
                size_t pos = 0;
                std::string token;
                while ((pos = customStrings.find(';')) != std::string::npos) {
                    token = customStrings.substr(0, pos);
                    parameter.options.push_back(token);
                    customStrings.erase(0, pos + 1);
                }
                // Push the last token
                if (!customStrings.empty()) {
                    parameter.options.push_back(customStrings);
                }
            }
            break;
        default:
            parameter.valueType = ParameterType::NoValue;
            return std::nullopt;
    }

    return parameter;
}

inline int SierraChartHelpers::GetMaxInputs() {
    return sizeof(((s_sc*)0)->Input) / sizeof(s_SCInput_145);
}

inline DateTime SierraChartHelpers::ConvertToDateTime(const SCDateTime& scDateTime) {
    return DateTime(std::chrono::system_clock::from_time_t(scDateTime.ToUNIXTime()));
}

inline TradeStatistics SierraChartHelpers::MapToTradeStatistics(const n_ACSIL::s_TradeStatistics& scStats) {
    TradeStatistics stats;
    stats.profit = scStats.ClosedTradesProfitLoss;
    stats.maxDrawdown = scStats.MaximumDrawdown;
    stats.winRate = scStats.PercentProfitable;
    stats.totalTrades = scStats.TotalTrades;
    stats.winningTrades = scStats.WinningTrades;
    stats.losingTrades = scStats.LosingTrades;
    stats.averageWin = scStats.AverageWinningTrade;
    stats.averageLoss = scStats.AverageLosingTrade;
    stats.profitFactor = scStats.ProfitFactor;
    stats.maxConsecutiveWins = scStats.MaxConsecutiveWinners;
    stats.maxConsecutiveLosses = scStats.MaxConsecutiveLosers;
    stats.averageTradeDuration = scStats.AverageTimeInTrades;
    stats.largestWin = scStats.LargestWinningTrade;
    stats.largestLoss = scStats.LargestLosingTrade;
    stats.sharpeRatio = scStats.AverageProfitFactor; // Assuming it's available
    stats.sortinoRatio = 0; // Needs to be calculated or added to s_TradeStatistics
    stats.calmarRatio = 0; // Needs to be calculated or added to s_TradeStatistics

    stats.closedProfit = scStats.ClosedTradesTotalProfit;
    stats.closedLoss = scStats.ClosedTradesTotalLoss;
    stats.totalCommission = scStats.TotalCommissions;
    stats.maximumRunup = scStats.MaximumRunup;
    stats.maximumTradeRunup = scStats.MaximumFlatToFlatTradeOpenProfit;
    stats.maximumTradeDrawdown = scStats.MaximumFlatToFlatTradeOpenLoss;
    stats.maximumOpenPositionProfit = scStats.MaximumTradeOpenProfit;
    stats.maximumOpenPositionLoss = scStats.MaximumTradeOpenLoss;
    stats.totalLongTrades = scStats.LongTrades;
    stats.totalShortTrades = scStats.ShortTrades;
    stats.totalWinningQuantity = scStats.WinningQuantity;
    stats.totalLosingQuantity = scStats.LosingQuantity;
    stats.totalFilledQuantity = scStats.TotalFilledQuantity;
    stats.largestTradeQuantity = scStats.LargestTradeQuantity;
    stats.timeInWinningTrades = scStats.AverageTimeInWinningTrades;
    stats.timeInLosingTrades = scStats.AverageTimeInLosingTrades;
    stats.maxConsecutiveWinners = scStats.MaxConsecutiveWinners;
    stats.maxConsecutiveLosers = scStats.MaxConsecutiveLosers;
    stats.lastTradeProfitLoss = scStats.LastTradeProfitLoss;
    stats.lastTradeQuantity = scStats.LastTradeQuantity;
    stats.lastFillDateTime = ConvertToDateTime(scStats.LastFillDateTime);
    stats.lastEntryDateTime = ConvertToDateTime(scStats.LastEntryDateTime);
    stats.lastExitDateTime = ConvertToDateTime(scStats.LastExitDateTime);
    stats.totalBuyQuantity = scStats.TotalBuyQuantity;
    stats.totalSellQuantity = scStats.TotalSellQuantity;

    return stats;
}

inline PositionData SierraChartHelpers::MapToPositionData(const s_SCPositionData& scPositionData) {
    PositionData positionData;    
    positionData.symbol = scPositionData.Symbol.GetChars();
    positionData.positionQuantity = scPositionData.PositionQuantity;
    positionData.averagePrice = scPositionData.AveragePrice;
    positionData.openProfitLoss = scPositionData.OpenProfitLoss;
    positionData.cumulativeProfitLoss = scPositionData.CumulativeProfitLoss;
    positionData.winTrades = scPositionData.WinTrades;
    positionData.loseTrades = scPositionData.LoseTrades;
    positionData.totalTrades = scPositionData.TotalTrades;
    positionData.maximumOpenPositionLoss = scPositionData.MaximumOpenPositionLoss;
    positionData.maximumOpenPositionProfit = scPositionData.MaximumOpenPositionProfit;
    positionData.lastTradeProfitLoss = scPositionData.LastTradeProfitLoss;
    positionData.positionQuantityWithAllWorkingOrders = scPositionData.PositionQuantityWithAllWorkingOrders;
    positionData.positionQuantityWithExitWorkingOrders = scPositionData.PositionQuantityWithExitWorkingOrders;
    positionData.workingOrdersExist = scPositionData.WorkingOrdersExist;
    positionData.dailyProfitLoss = scPositionData.DailyProfitLoss;
    positionData.lastFillDateTime = ConvertToDateTime(scPositionData.LastFillDateTime);
    positionData.lastEntryDateTime = ConvertToDateTime(scPositionData.LastEntryDateTime);
    positionData.lastExitDateTime = ConvertToDateTime(scPositionData.LastExitDateTime);
    positionData.priceHighDuringPosition = scPositionData.PriceHighDuringPosition;
    positionData.dailyTotalQuantityFilled = scPositionData.DailyTotalQuantityFilled;
    positionData.priceLowDuringPosition = scPositionData.PriceLowDuringPosition;
    positionData.priorPositionQuantity = scPositionData.PriorPositionQuantity;
    positionData.positionQuantityWithExitMarketOrders = scPositionData.PositionQuantityWithExitMarketOrders;
    positionData.totalQuantityFilled = scPositionData.TotalQuantityFilled;
    positionData.lastTradeQuantity = scPositionData.LastTradeQuantity;
    positionData.tradeStatsDailyProfitLoss = scPositionData.TradeStatsDailyProfitLoss;
    positionData.tradeStatsDailyClosedQuantity = scPositionData.TradeStatsDailyClosedQuantity;
    positionData.tradeStatsOpenProfitLoss = scPositionData.TradeStatsOpenProfitLoss;
    positionData.tradeStatsOpenQuantity = scPositionData.TradeStatsOpenQuantity;
    positionData.nonAttachedWorkingOrdersExist = scPositionData.NonAttachedWorkingOrdersExist;
    positionData.tradeAccount = scPositionData.TradeAccount.GetChars();
    positionData.positionQuantityWithMarketWorkingOrders = scPositionData.PositionQuantityWithMarketWorkingOrders;
    positionData.positionQuantityWithAllWorkingOrdersExceptNonMarketExits = scPositionData.PositionQuantityWithAllWorkingOrdersExceptNonMarketExits;
    positionData.positionQuantityWithAllWorkingOrdersExceptAllExits = scPositionData.PositionQuantityWithAllWorkingOrdersExceptAllExits;
    positionData.allWorkingBuyOrdersQuantity = scPositionData.AllWorkingBuyOrdersQuantity;
    positionData.allWorkingSellOrdersQuantity = scPositionData.AllWorkingSellOrdersQuantity;

    return positionData;
}

inline TimeAndSales SierraChartHelpers::MapToTimeAndSales(const s_TimeAndSales& tsData, s_sc* sc) {
        TimeAndSales ts;
        ts.dateTime = DateTime(tsData.DateTime.ToUNIXTime());
        ts.price = tsData.Price * sc->RealTimePriceMultiplier;
        ts.volume = tsData.Volume;
        ts.bid = tsData.Bid * sc->RealTimePriceMultiplier;
        ts.ask = tsData.Ask * sc->RealTimePriceMultiplier;
        ts.bidSize = tsData.BidSize;
        ts.askSize = tsData.AskSize;
        ts.type = static_cast<TimeAndSalesType>(tsData.Type);
        return ts;
    }

#endif // SIERRACHARTHELPERS_H
