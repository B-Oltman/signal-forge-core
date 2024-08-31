#ifndef COMMONTYPES_H
#define COMMONTYPES_H

#include <string>
#include <vector>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <variant>
#include <optional>
#include <utility>
#include <nlohmann/json.hpp>
#include <atomic>

class ParameterManager;

// Function to get trade system name
std::string GetTradeSystemName();

#include <chrono>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <string>

template <typename T>
T Min(const T &a, const T &b)
{
    return (a < b) ? a : b;
}

template <typename T>
T Max(const T &a, const T &b)
{
    return (a > b) ? a : b;
}

struct DateTime
{
    std::chrono::system_clock::time_point timePoint;

    DateTime() : timePoint(std::chrono::system_clock::now()) {}

    explicit DateTime(const std::chrono::system_clock::time_point &tp) : timePoint(tp) {}

    // Constructor that assumes time_t is in UTC
    explicit DateTime(time_t time)
    {
        // TODO: Fix time handling, issue is sierra chart spits out a local time point which would normally be interacted with as UTC.
        // Instead, the system will operate on it like its utc, but we know it's cst so we have to handle daylight savings etc manually.
        // Better solution would be to ensure all dates come in as UTC even from sierra chart, maybe will do this, maybe this method will work fine.

        // Normally gmttime() would return utc, but since the date is already in utc, it returns cst.

        // Convert time_t to a tm structure assuming it's in UTC
        std::tm utcTm = *std::gmtime(&time);

        // Adjust for Central Time manually (UTC-6 for CST, UTC-5 for CDT)
        utcTm.tm_hour -= (isDaylightSavingTime(utcTm) ? 1 : 0);

        // Ensure the tm structure correctly wraps around when subtracting hours
        std::mktime(&utcTm); // Normalize the tm structure

        // Convert the adjusted tm structure to time_t
        std::time_t adjustedTime = std::mktime(&utcTm);

        // Convert the adjusted time_t to a time_point
        this->timePoint = std::chrono::system_clock::from_time_t(adjustedTime);
    }

    // Helper function to check if Daylight Saving Time is in effect
    static bool isDaylightSavingTime(const std::tm &tm)
    {
        // std::mktime sets tm_isdst to positive if DST is in effect
        std::tm localTm = tm;  // Make a copy to preserve the original
        std::mktime(&localTm); // Normalize and set tm_isdst
        return localTm.tm_isdst > 0;
    }

    // Convert time_point back to time_t, handling DST
    std::time_t ToTimeT() const
    {
        // Convert time_point to time_t (UTC)
        std::time_t utcTime = std::chrono::system_clock::to_time_t(timePoint);

        // Convert to tm structure (still in UTC)
        std::tm utcTm = *std::gmtime(&utcTime);

        // Reverse the Central Time adjustment (add 6 hours for CST, 5 hours for CDT)
        utcTm.tm_hour += (isDaylightSavingTime(utcTm) ? 1 : 0);

        // Normalize the tm structure after adjustment
        std::mktime(&utcTm); // Normalize the tm structure

        // Convert the adjusted tm structure back to time_t
        std::time_t localTime = std::mktime(&utcTm);

        return localTime;
    }

    // Convert time_point to a string in CST/CDT
    std::string ToString() const
    {
        // Convert time_point to time_t (UTC)
        std::time_t utcTime = std::chrono::system_clock::to_time_t(timePoint);

        // Convert to a tm structure (still in UTC)
        std::tm utcTm = *std::gmtime(&utcTime);                

        // Normalize the tm structure after adjustment
        std::mktime(&utcTm); // Normalize the tm structure

        // Create a string buffer for the formatted time string
        char buffer[26];

#ifdef _WIN32
        asctime_s(buffer, sizeof(buffer), &utcTm);
#else
        asctime_r(&utcTm, buffer);
#endif

        // Convert to std::string and remove the newline character at the end
        std::string timeStr(buffer);
        if (!timeStr.empty() && timeStr.back() == '\n')
        {
            timeStr.pop_back();
        }

        return timeStr;
    }

    static DateTime FromTimeString(const std::string &timeStr)
    {
        // Get the current time and date in UTC
        std::time_t t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        std::tm utcTm = *std::gmtime(&t); // Convert time_t to UTC tm

        // Parse the time string (assumes format "HH:MM" or "HH:MM:SS")
        std::istringstream ss(timeStr);
        ss >> std::get_time(&utcTm, "%H:%M");

        // If seconds are also provided
        if (timeStr.size() > 5)
        {
            ss.clear();
            ss.seekg(0);
            ss >> std::get_time(&utcTm, "%H:%M:%S");
        }

        // Adjust for Central Time manually (UTC-6 for CST, UTC-5 for CDT)
        utcTm.tm_hour -= (isDaylightSavingTime(utcTm) ? 1 : 0);

        // Convert the adjusted tm structure to time_t
        std::time_t adjustedTime = std::mktime(&utcTm);

        // Now, create a DateTime object assuming the adjusted time
        return DateTime(adjustedTime);
    }

    static DateTime FromString(const std::string &dateTimeStr)
    {
        std::tm utcTm = {};
        std::istringstream ss(dateTimeStr);

        ss >> std::get_time(&utcTm, "%a, %d %b %Y %H:%M:%S");

        if (ss.fail())
        {
            throw std::runtime_error("Failed to parse date time string");
        }

        // Adjust for Central Time manually (UTC-6 for CST, UTC-5 for CDT)
        utcTm.tm_hour -= (isDaylightSavingTime(utcTm) ? 1 : 0);

        // Convert the adjusted tm structure to time_t
        std::time_t adjustedTime = std::mktime(&utcTm);

        // Now, create a DateTime object assuming the adjusted time
        return DateTime(adjustedTime);
    }

    bool operator<=(const DateTime &other) const
    {
        return timePoint <= other.timePoint;
    }

    bool operator>=(const DateTime &other) const
    {
        return timePoint >= other.timePoint;
    }

    bool operator<(const DateTime &other) const
    {
        return timePoint < other.timePoint;
    }

    bool operator>(const DateTime &other) const
    {
        return timePoint > other.timePoint;
    }

    DateTime operator+(const std::chrono::minutes &minutes) const
    {
        return DateTime(timePoint + minutes);
    }

    DateTime &operator+=(const std::chrono::minutes &minutes)
    {
        timePoint += minutes;
        return *this;
    }

    std::chrono::duration<double> operator-(const DateTime &other) const
    {
        return timePoint - other.timePoint;
    }

    DateTime &operator-=(const std::chrono::minutes &minutes)
    {
        timePoint -= minutes;
        return *this;
    }

    DateTime AddDays(int days) const
    {
        return DateTime(timePoint + std::chrono::hours(days * 24));
    }

    DateTime AddMinutes(int minutes) const
    {
        return DateTime(timePoint + std::chrono::minutes(minutes));
    }

    DateTime AddSeconds(int seconds) const
    {
        return DateTime(timePoint + std::chrono::seconds(seconds));
    }

    DateTime AddHours(int hours) const
    {
        return DateTime(timePoint + std::chrono::hours(hours));
    }

    DateTime AddMilliseconds(int milliseconds) const
    {
        return DateTime(timePoint + std::chrono::milliseconds(milliseconds));
    }

    DateTime AddMicroseconds(int microseconds) const
    {
        return DateTime(timePoint + std::chrono::microseconds(microseconds));
    }

    std::string GetDayOfWeek() const
    {
        std::time_t time = ToTimeT();
        std::tm *tm = std::localtime(&time);

        static const char *daysOfWeek[] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
        return std::string(daysOfWeek[tm->tm_wday]);
    }

    std::string GetTime() const
    {
        std::time_t time = ToTimeT();
        std::tm *tm = std::localtime(&time);

        std::ostringstream oss;
        oss << std::setw(2) << std::setfill('0') << tm->tm_hour << ":"
            << std::setw(2) << std::setfill('0') << tm->tm_min << ":"
            << std::setw(2) << std::setfill('0') << tm->tm_sec;
        return oss.str();
    }
};

struct UniqueIDGenerator
{
    static std::atomic<int> nextId;

    static std::string GenerateID()
    {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        std::tm tm;
#ifdef _WIN32
        localtime_s(&tm, &time);
#else
        localtime_r(&time, &tm);
#endif
        char buffer[20];
        strftime(buffer, sizeof(buffer), "%Y%m%d%H%M%S", &tm);
        int id = nextId++;
        return std::string(buffer) + "_" + std::to_string(id);
    }
};

enum class OrderType
{
    Market,
    Limit,
    Stop,
    StopLimit,
    MarketIfTouched,
    LimitChase,
    LimitTouchChase,
    TrailingStop,
    TrailingStopLimit,
    TriggeredTrailingStop3Offsets,
    TriggeredTrailingStopLimit3Offsets,
    StepTrailingStop,
    StepTrailingStopLimit,
    TriggeredStepTrailingStop,
    TriggeredStepTrailingStopLimit,
    OcoLimitStop,
    OcoLimitStopLimit,
    OcoBuyStopSellStop,
    OcoBuyStopLimitSellStopLimit,
    OcoBuyLimitSellLimit,
    LimitIfTouched,
    BidAskQuantityTriggeredStop,
    TriggeredLimit,
    TradeVolumeTriggeredStop,
    StopWithBidAskTriggering,
    StopWithLastTriggering,
    LimitIfTouchedClientSide,
    MarketIfTouchedClientSide,
    TradeVolumeTriggeredStopLimit,
    StopLimitClientSide,
    TriggeredStop,
    TriggeredLimitWithChase,
    MarketLimit,
    NumOrders
};

enum class OrderStatus
{
    PartiallyFilled,
    Filled,
    Cancelled,
    Rejected,
    None
};

enum class OrderDirection
{
    Buy,
    Sell,
    None
};

struct RiskAssessment
{
    bool acceptable;
    std::string assessmentNote;
    double maxDrawdown;
    double volatility;
    double valueAtRisk;
    double expectedShortfall;
    double sharpeRatio;
    double sortinoRatio;
    double beta;
    double alpha;
    double correlation;

    RiskAssessment()
        : acceptable(false), maxDrawdown(0.0), volatility(0.0), valueAtRisk(0.0),
          expectedShortfall(0.0), sharpeRatio(0.0), sortinoRatio(0.0), beta(0.0),
          alpha(0.0), correlation(0.0) {}

    nlohmann::json ToJson() const
    {
        return {
            {"acceptable", acceptable},
            {"assessmentNote", assessmentNote},
            {"maxDrawdown", maxDrawdown},
            {"volatility", volatility},
            {"valueAtRisk", valueAtRisk},
            {"expectedShortfall", expectedShortfall},
            {"sharpeRatio", sharpeRatio},
            {"sortinoRatio", sortinoRatio},
            {"beta", beta},
            {"alpha", alpha},
            {"correlation", correlation}};
    }

    static RiskAssessment FromJson(const nlohmann::json &j)
    {
        RiskAssessment ra;
        ra.acceptable = j.at("acceptable").get<bool>();
        ra.assessmentNote = j.at("assessmentNote").get<std::string>();
        ra.maxDrawdown = j.at("maxDrawdown").get<double>();
        ra.volatility = j.at("volatility").get<double>();
        ra.valueAtRisk = j.at("valueAtRisk").get<double>();
        ra.expectedShortfall = j.at("expectedShortfall").get<double>();
        ra.sharpeRatio = j.at("sharpeRatio").get<double>();
        ra.sortinoRatio = j.at("sortinoRatio").get<double>();
        ra.beta = j.at("beta").get<double>();
        ra.alpha = j.at("alpha").get<double>();
        ra.correlation = j.at("correlation").get<double>();
        return ra;
    }

    std::string ToString() const
    {
        return "RiskAssessment [Acceptable: " + std::to_string(acceptable) +
               ", Assessment Note: " + assessmentNote +
               ", Max Drawdown: " + std::to_string(maxDrawdown) +
               ", Volatility: " + std::to_string(volatility) +
               ", Value At Risk: " + std::to_string(valueAtRisk) +
               ", Expected Shortfall: " + std::to_string(expectedShortfall) +
               ", Sharpe Ratio: " + std::to_string(sharpeRatio) +
               ", Sortino Ratio: " + std::to_string(sortinoRatio) +
               ", Beta: " + std::to_string(beta) +
               ", Alpha: " + std::to_string(alpha) +
               ", Correlation: " + std::to_string(correlation) + "]";
    }
};

struct PendingOrder
{
    int orderId;
    int signalWeight;
    std::string symbol;
    std::string tradeAccount;
    double quantity;
    double price;
    DateTime dateTime;
    OrderType orderType;
    OrderDirection direction;
    RiskAssessment orderRisk;
    std::optional<std::vector<PendingOrder>> attachedOrders;
};

struct ExecutedOrder
{
    int orderId;
    double entryPrice = 0.0;
    double stopLoss = 0.0;
    double takeProfit = 0.0;
    double filledQuantity = 0.0;
    double requestedQuantity = 0.0;
    RiskAssessment orderRisk;
    DateTime entryTime;
    DateTime exitTime;
    OrderStatus status = OrderStatus::None;
    OrderDirection direction = OrderDirection::None;

    std::string ToString() const
    {
        return ("ExecutedOrder [OrderID: " + std::to_string(orderId) +
                ", EntryPrice: " + std::to_string(entryPrice) +
                ", StopLoss: " + std::to_string(stopLoss) +
                ", TakeProfit: " + std::to_string(takeProfit) +
                ", Filled Quantity: " + std::to_string(filledQuantity) +
                ", Requested Quantity: " + std::to_string(requestedQuantity) +
                ", OrderRisk: " + orderRisk.ToString() +
                ", EntryTime: " + entryTime.ToString() +
                ", ExitTime: " + exitTime.ToString() +
                ", Direction: " + std::to_string(static_cast<int>(direction)) +
                ", Status: " + std::to_string(static_cast<int>(status)) + "]");
    }
};

struct BarData
{
    DateTime startTime;
    DateTime endTime;
    double open;
    double high;
    double low;
    double close;
    std::optional<double> numberOfTrades;
    std::optional<double> openInterest;
    std::optional<double> ohlcAvg;
    std::optional<double> hlcAvg;
    std::optional<double> hlAvg;
    std::optional<double> volume;
    std::optional<double> bidVolume;
    std::optional<double> askVolume;
    std::optional<double> upVolume;
    std::optional<double> downVolume;
    std::optional<double> bidTrades;
    std::optional<double> askTrades;

    BarData(
        DateTime st, DateTime et, double o, double h, double l, double c, std::optional<double> v,
        std::optional<double> numTrades = std::nullopt, std::optional<double> openInt = std::nullopt,
        std::optional<double> ohlcA = std::nullopt, std::optional<double> hlcA = std::nullopt,
        std::optional<double> hlA = std::nullopt, std::optional<double> bidVol = std::nullopt,
        std::optional<double> askVol = std::nullopt, std::optional<double> upVol = std::nullopt,
        std::optional<double> downVol = std::nullopt, std::optional<double> bidTr = std::nullopt,
        std::optional<double> askTr = std::nullopt)
        : startTime(st), endTime(et), open(o), high(h), low(l), close(c), volume(v),
          numberOfTrades(numTrades), openInterest(openInt), ohlcAvg(ohlcA),
          hlcAvg(hlcA), hlAvg(hlA), bidVolume(bidVol), askVolume(askVol),
          upVolume(upVol), downVolume(downVol), bidTrades(bidTr), askTrades(askTr) {}

    std::string ToString() const
    {
        return "BarData [StartTime: " + startTime.ToString() +
               "  EndTime: " + endTime.ToString() +
               ", Open: " + std::to_string(open) +
               ", High: " + std::to_string(high) +
               ", Low: " + std::to_string(low) +
               ", Close: " + std::to_string(close) +
               (volume ? ", Volume: " + std::to_string(*volume) : "") +
               (numberOfTrades ? ", Number of Trades: " + std::to_string(*numberOfTrades) : "") +
               (openInterest ? ", Open Interest: " + std::to_string(*openInterest) : "") +
               (ohlcAvg ? ", OHLC Avg: " + std::to_string(*ohlcAvg) : "") +
               (hlcAvg ? ", HLC Avg: " + std::to_string(*hlcAvg) : "") +
               (hlAvg ? ", HL Avg: " + std::to_string(*hlAvg) : "") +
               (bidVolume ? ", Bid Volume: " + std::to_string(*bidVolume) : "") +
               (askVolume ? ", Ask Volume: " + std::to_string(*askVolume) : "") +
               (upVolume ? ", Up Volume: " + std::to_string(*upVolume) : "") +
               (downVolume ? ", Down Volume: " + std::to_string(*downVolume) : "") +
               (bidTrades ? ", Bid Trades: " + std::to_string(*bidTrades) : "") +
               (askTrades ? ", Ask Trades: " + std::to_string(*askTrades) : "") + "]";
    }
};

enum class ParameterType
{
    Index,
    Float,
    Boolean,
    DateTime,
    Integer,
    Color,
    String,
    CustomString,
    Double,
    TimeWithTimeZone,
    NoValue
};

using ParameterValue = std::variant<int, float, bool, double, std::string, std::pair<double, int>, unsigned int, std::monostate>;

template <typename T>
T GetParameterValue(const ParameterValue &paramValue)
{
    if (auto val = std::get_if<T>(&paramValue))
    {
        return *val;
    }
    else
    {
        throw std::bad_variant_access();
    }
}

template <typename T>
T GetParameterValue(const ParameterValue &paramValue, T defaultValue)
{
    try
    {
        return GetParameterValue<T>(paramValue);
    }
    catch (const std::bad_variant_access &)
    {
        return defaultValue;
    }
}

struct TradeSystemParameter
{
    std::string key;
    std::string name;
    std::string tradeSystemName = GetTradeSystemName(); // Assign from function
    ParameterType valueType;
    ParameterValue value;
    std::variant<int, float, double> minValue;
    std::variant<int, float, double> maxValue;
    std::vector<std::string> options;
    bool restrictAutoTuning = false;

    TradeSystemParameter() : valueType(ParameterType::NoValue), value(0), minValue(0), maxValue(0) {}
    TradeSystemParameter(std::string key, std::string name, ParameterType type)
        : key(std::move(key)), name(std::move(name)), valueType(type), value(0), minValue(0), maxValue(0) {}

    void Clear()
    {
        value = 0;
        minValue = 0;
        maxValue = 0;
        options.clear();
    }

    template <typename T>
    void SetValue(T newValue)
    {
        value = std::move(newValue);
    }

    template <typename T>
    T GetValue() const
    {
        return GetParameterValue<T>(value);
    }

    template <typename T>
    T GetValue(T defaultValue) const
    {
        return GetParameterValue<T>(value, defaultValue);
    }

    template <typename T>
    void SetMinValue(T newMinValue)
    {
        minValue = std::move(newMinValue);
    }

    template <typename T>
    T GetMinValue() const
    {
        return std::get<T>(minValue);
    }

    template <typename T>
    void SetMaxValue(T newMaxValue)
    {
        maxValue = std::move(newMaxValue);
    }

    template <typename T>
    T GetMaxValue() const
    {
        return std::get<T>(maxValue);
    }

    void SetOptions(const std::vector<std::string> &newOptions)
    {
        options = newOptions;
    }

    const std::vector<std::string> &GetOptions() const
    {
        return options;
    }

    nlohmann::json ToJson() const
    {
        nlohmann::json j;
        j["key"] = key;
        j["name"] = name;
        j["tradeSystemName"] = tradeSystemName; // Add tradeSystemName to JSON
        j["valueType"] = static_cast<int>(valueType);

        std::visit([&j](auto &&arg)
                   { 
        if constexpr (std::is_same_v<std::decay_t<decltype(arg)>, std::monostate>) {
            j["value"] = nullptr;
        } else {
            j["value"] = arg;
        } }, value);

        std::visit([&j](auto &&arg)
                   { 
        if constexpr (std::is_same_v<std::decay_t<decltype(arg)>, std::monostate>) {
            j["minValue"] = nullptr;
        } else {
            j["minValue"] = arg;
        } }, minValue);

        std::visit([&j](auto &&arg)
                   { 
        if constexpr (std::is_same_v<std::decay_t<decltype(arg)>, std::monostate>) {
            j["maxValue"] = nullptr;
        } else {
            j["maxValue"] = arg;
        } }, maxValue);

        j["options"] = options;
        j["restrictAutoTuning"] = restrictAutoTuning;

        return j;
    }

    static TradeSystemParameter FromJson(const nlohmann::json &j)
    {
        TradeSystemParameter param;
        param.key = j.at("key").get<std::string>();
        param.name = j.at("name").get<std::string>();
        param.tradeSystemName = j.at("tradeSystemName").get<std::string>(); // Add tradeSystemName
        param.valueType = static_cast<ParameterType>(j.at("valueType").get<int>());

        switch (param.valueType)
        {
        case ParameterType::Float:
            param.value = j.at("value").get<float>();
            break;
        case ParameterType::Integer:
            param.value = j.at("value").get<int>();
            break;
        case ParameterType::Boolean:
            param.value = j.at("value").get<bool>();
            break;
        case ParameterType::String:
            param.value = j.at("value").get<std::string>();
            break;
        case ParameterType::Double:
            param.value = j.at("value").get<double>();
            break;
        default:
            break;
        }

        if (j.contains("minValue"))
        {
            if (j["minValue"].is_number_float())
            {
                param.minValue = j.at("minValue").get<double>();
            }
            else if (j["minValue"].is_number_integer())
            {
                param.minValue = j.at("minValue").get<int>();
            }
        }

        if (j.contains("maxValue"))
        {
            if (j["maxValue"].is_number_float())
            {
                param.maxValue = j.at("maxValue").get<double>();
            }
            else if (j["maxValue"].is_number_integer())
            {
                param.maxValue = j.at("maxValue").get<int>();
            }
        }

        if (j.contains("options"))
        {
            param.options = j.at("options").get<std::vector<std::string>>();
        }

        if (j.contains("restrictAutoTuning"))
        {
            param.restrictAutoTuning = j.at("restrictAutoTuning").get<bool>();
        }

        return param;
    }
};

namespace nlohmann
{
    template <>
    struct adl_serializer<TradeSystemParameter>
    {
        static void to_json(json &j, const TradeSystemParameter &param)
        {
            j = param.ToJson();
        }

        static void from_json(const json &j, TradeSystemParameter &param)
        {
            param = TradeSystemParameter::FromJson(j);
        }
    };
}

struct TradeSystemParameterGroup
{
    std::string id; // This should be set by the python server database stuff
    DateTime lastUpdated;
    std::unordered_map<std::string, TradeSystemParameter> parameters;
    std::string tradeSystemName;

    TradeSystemParameterGroup() : lastUpdated(DateTime()) {}
    TradeSystemParameterGroup(DateTime lastUpdated, std::unordered_map<std::string, TradeSystemParameter> parameters, std::string tradeSystemName)
        : lastUpdated(lastUpdated), parameters(parameters), tradeSystemName(tradeSystemName) {}

    void AddParameter(const TradeSystemParameter &param)
    {
        parameters[param.key] = param;
    }

    std::optional<TradeSystemParameter> GetParameter(const std::string &key) const
    {
        auto it = parameters.find(key);
        if (it != parameters.end())
        {
            return it->second;
        }
        return std::nullopt;
    }

    std::vector<TradeSystemParameter> GetParameters() const
    {
        std::vector<TradeSystemParameter> params;
        for (const auto &pair : parameters)
        {
            params.push_back(pair.second);
        }
        return params;
    }

    void RemoveParameter(const std::string &key)
    {
        parameters.erase(key);
    }

    nlohmann::json ToJson() const
    {
        std::vector<nlohmann::json> parametersJson;
        for (const auto &pair : parameters)
        {
            parametersJson.push_back(pair.second.ToJson());
        }

        return {
            {"id", id},
            {"lastUpdated", lastUpdated.ToString()},
            {"parameters", parametersJson},
            {"tradeSystemName", tradeSystemName} // Add tradeSystemName to JSON
        };
    }

    static TradeSystemParameterGroup FromJson(const nlohmann::json &j)
    {
        TradeSystemParameterGroup group;
        group.id = j.at("id").get<std::string>();
        group.lastUpdated = DateTime::FromString(j.at("lastUpdated").get<std::string>());

        for (const auto &paramJson : j.at("parameters"))
        {
            auto param = TradeSystemParameter::FromJson(paramJson);
            group.parameters[param.key] = param;
        }

        group.tradeSystemName = j.at("tradeSystemName").get<std::string>(); // Add tradeSystemName
        return group;
    }
};

namespace nlohmann
{
    template <>
    struct adl_serializer<TradeSystemParameterGroup>
    {
        static void to_json(json &j, const TradeSystemParameterGroup &group)
        {
            j = group.ToJson();
        }

        static void from_json(const json &j, TradeSystemParameterGroup &group)
        {
            group = TradeSystemParameterGroup::FromJson(j);
        }
    };
}

struct TradeSignal
{
    std::string signalKey = "TRADE_SIGNAL";
    std::string id;
    bool buySignal = false;
    bool sellSignal = false;
    double price = 0.0;
    double stopLoss = 0.0;
    double takeProfit = 0.0;
    double quantity = 0.0;
    double signalWeight = 1.0;
    std::vector<std::string> attachedSignalIds;
    std::string tradeSystemName = GetTradeSystemName(); // Assign from function

    TradeSignal() : id(UniqueIDGenerator::GenerateID()) {}

    std::string ToString() const
    {
        std::ostringstream os;
        os << "TradeSignal [SignalKey: " << signalKey
           << ", ID: " << id
           << ", Buy: " << buySignal
           << ", Sell: " << sellSignal
           << ", Price: " << price
           << ", StopLoss: " << stopLoss
           << ", TakeProfit: " << takeProfit
           << ", Quantity: " << quantity
           << ", SignalWeight: " << signalWeight
           << ", AttachedSignals: [";

        for (const auto &attachedSignal : attachedSignalIds)
        {
            os << attachedSignal << ", ";
        }

        std::string result = os.str();
        if (!attachedSignalIds.empty())
        {
            result = result.substr(0, result.length() - 2);
        }

        result += "]]";
        return result;
    }

    nlohmann::json ToJson() const
    {
        return {
            {"signalKey", signalKey},
            {"id", id},
            {"buySignal", buySignal},
            {"sellSignal", sellSignal},
            {"price", price},
            {"stopLoss", stopLoss},
            {"takeProfit", takeProfit},
            {"quantity", quantity},
            {"signalWeight", signalWeight},
            {"attachedSignalIds", attachedSignalIds},
            {"tradeSystemName", tradeSystemName} // Add tradeSystemName to JSON
        };
    }

    std::string ToJsonString() const
    {
        return ToJson().dump();
    }

    static TradeSignal FromJson(const nlohmann::json &j)
    {
        TradeSignal signal;
        signal.signalKey = j.at("signalKey").get<std::string>();
        signal.id = j.at("id").get<std::string>();
        signal.buySignal = j.at("buySignal").get<bool>();
        signal.sellSignal = j.at("sellSignal").get<bool>();
        signal.price = j.at("price").get<double>();
        signal.stopLoss = j.at("stopLoss").get<double>();
        signal.takeProfit = j.at("takeProfit").get<double>();
        signal.quantity = j.at("quantity").get<double>();
        signal.signalWeight = j.at("signalWeight").get<double>();
        signal.attachedSignalIds = j.at("attachedSignalIds").get<std::vector<std::string>>();
        signal.tradeSystemName = j.at("tradeSystemName").get<std::string>(); // Add tradeSystemName
        return signal;
    }

    static TradeSignal FromJsonString(const std::string &jsonString)
    {
        auto j = nlohmann::json::parse(jsonString);
        return FromJson(j);
    }
};

enum class ContextType
{
    LiveTrading,
    PaperTrading,
    Backtesting
};

enum class Mode
{
    Asynchronous,
    Synchronous
};

struct MarketContextData
{
    std::string symbol;
    double price;
    double volume;
    double sentimentScore;
};

extern const std::vector<std::string> MovingAverageTypes;
extern const std::vector<std::string> TimePeriods;

struct TradeStatistics
{
    std::string id;
    double profit;
    double maxDrawdown;
    double winRate;
    int totalTrades;
    int winningTrades;
    int losingTrades;
    double averageWin;
    double averageLoss;
    double profitFactor;
    int maxConsecutiveWins;
    int maxConsecutiveLosses;
    double averageTradeDuration;
    double largestWin;
    double largestLoss;
    double sharpeRatio;
    double sortinoRatio;
    double calmarRatio;

    double closedProfit;
    double closedLoss;
    double totalCommission;
    double maximumRunup;
    double maximumTradeRunup;
    double maximumTradeDrawdown;
    double maximumOpenPositionProfit;
    double maximumOpenPositionLoss;
    int totalLongTrades;
    int totalShortTrades;
    double totalWinningQuantity;
    double totalLosingQuantity;
    double totalFilledQuantity;
    double largestTradeQuantity;
    int timeInWinningTrades;
    int timeInLosingTrades;
    int maxConsecutiveWinners;
    int maxConsecutiveLosers;
    double lastTradeProfitLoss;
    double lastTradeQuantity;
    DateTime lastFillDateTime;
    DateTime lastEntryDateTime;
    DateTime lastExitDateTime;
    DateTime sessionEndDateTime;
    double totalBuyQuantity;
    double totalSellQuantity;
    std::string tradeSystemName = GetTradeSystemName(); // Assign from function

    TradeStatistics() : id(UniqueIDGenerator::GenerateID()) {}

    nlohmann::json ToJson() const
    {
        return {
            {"id", id},
            {"profit", profit},
            {"maxDrawdown", maxDrawdown},
            {"winRate", winRate},
            {"totalTrades", totalTrades},
            {"winningTrades", winningTrades},
            {"losingTrades", losingTrades},
            {"averageWin", averageWin},
            {"averageLoss", averageLoss},
            {"profitFactor", profitFactor},
            {"maxConsecutiveWins", maxConsecutiveWins},
            {"maxConsecutiveLosses", maxConsecutiveLosses},
            {"averageTradeDuration", averageTradeDuration},
            {"largestWin", largestWin},
            {"largestLoss", largestLoss},
            {"sharpeRatio", sharpeRatio},
            {"sortinoRatio", sortinoRatio},
            {"calmarRatio", calmarRatio},
            {"closedProfit", closedProfit},
            {"closedLoss", closedLoss},
            {"totalCommission", totalCommission},
            {"maximumRunup", maximumRunup},
            {"maximumTradeRunup", maximumTradeRunup},
            {"maximumTradeDrawdown", maximumTradeDrawdown},
            {"maximumOpenPositionProfit", maximumOpenPositionProfit},
            {"maximumOpenPositionLoss", maximumOpenPositionLoss},
            {"totalLongTrades", totalLongTrades},
            {"totalShortTrades", totalShortTrades},
            {"totalWinningQuantity", totalWinningQuantity},
            {"totalLosingQuantity", totalLosingQuantity},
            {"totalFilledQuantity", totalFilledQuantity},
            {"largestTradeQuantity", largestTradeQuantity},
            {"timeInWinningTrades", timeInWinningTrades},
            {"timeInLosingTrades", timeInLosingTrades},
            {"maxConsecutiveWinners", maxConsecutiveWinners},
            {"maxConsecutiveLosers", maxConsecutiveLosers},
            {"lastTradeProfitLoss", lastTradeProfitLoss},
            {"lastTradeQuantity", lastTradeQuantity},
            {"lastFillDateTime", lastFillDateTime.ToString()},
            {"lastEntryDateTime", lastEntryDateTime.ToString()},
            {"lastExitDateTime", lastExitDateTime.ToString()},
            {"sessionEndDateTime", sessionEndDateTime.ToString()},
            {"totalBuyQuantity", totalBuyQuantity},
            {"totalSellQuantity", totalSellQuantity},
            {"tradeSystemName", tradeSystemName} // Add tradeSystemName to JSON
        };
    }

    static TradeStatistics FromJson(const nlohmann::json &j)
    {
        TradeStatistics stats;
        stats.id = j.at("id").get<std::string>();
        stats.profit = j.at("profit").get<double>();
        stats.maxDrawdown = j.at("maxDrawdown").get<double>();
        stats.winRate = j.at("winRate").get<double>();
        stats.totalTrades = j.at("totalTrades").get<int>();
        stats.winningTrades = j.at("winningTrades").get<int>();
        stats.losingTrades = j.at("losingTrades").get<int>();
        stats.averageWin = j.at("averageWin").get<double>();
        stats.averageLoss = j.at("averageLoss").get<double>();
        stats.profitFactor = j.at("profitFactor").get<double>();
        stats.maxConsecutiveWins = j.at("maxConsecutiveWins").get<int>();
        stats.maxConsecutiveLosses = j.at("maxConsecutiveLosses").get<int>();
        stats.averageTradeDuration = j.at("averageTradeDuration").get<double>();
        stats.largestWin = j.at("largestWin").get<double>();
        stats.largestLoss = j.at("largestLoss").get<double>();
        stats.sharpeRatio = j.at("sharpeRatio").get<double>();
        stats.sortinoRatio = j.at("sortinoRatio").get<double>();
        stats.calmarRatio = j.at("calmarRatio").get<double>();
        stats.closedProfit = j.at("closedProfit").get<double>();
        stats.closedLoss = j.at("closedLoss").get<double>();
        stats.totalCommission = j.at("totalCommission").get<double>();
        stats.maximumRunup = j.at("maximumRunup").get<double>();
        stats.maximumTradeRunup = j.at("maximumTradeRunup").get<double>();
        stats.maximumTradeDrawdown = j.at("maximumTradeDrawdown").get<double>();
        stats.maximumOpenPositionProfit = j.at("maximumOpenPositionProfit").get<double>();
        stats.maximumOpenPositionLoss = j.at("maximumOpenPositionLoss").get<double>();
        stats.totalLongTrades = j.at("totalLongTrades").get<int>();
        stats.totalShortTrades = j.at("totalShortTrades").get<int>();
        stats.totalWinningQuantity = j.at("totalWinningQuantity").get<double>();
        stats.totalLosingQuantity = j.at("totalLosingQuantity").get<double>();
        stats.totalFilledQuantity = j.at("totalFilledQuantity").get<double>();
        stats.largestTradeQuantity = j.at("largestTradeQuantity").get<double>();
        stats.timeInWinningTrades = j.at("timeInWinningTrades").get<int>();
        stats.timeInLosingTrades = j.at("timeInLosingTrades").get<int>();
        stats.maxConsecutiveWinners = j.at("maxConsecutiveWinners").get<int>();
        stats.maxConsecutiveLosers = j.at("maxConsecutiveLosers").get<int>();
        stats.lastTradeProfitLoss = j.at("lastTradeProfitLoss").get<double>();
        stats.lastTradeQuantity = j.at("lastTradeQuantity").get<double>();
        stats.lastFillDateTime = DateTime::FromString(j.at("lastFillDateTime").get<std::string>());
        stats.lastEntryDateTime = DateTime::FromString(j.at("lastEntryDateTime").get<std::string>());
        stats.lastExitDateTime = DateTime::FromString(j.at("lastExitDateTime").get<std::string>());
        stats.sessionEndDateTime = DateTime::FromString(j.at("sessionEndDateTime").get<std::string>());
        stats.totalBuyQuantity = j.at("totalBuyQuantity").get<double>();
        stats.totalSellQuantity = j.at("totalSellQuantity").get<double>();
        stats.tradeSystemName = j.at("tradeSystemName").get<std::string>(); // Add tradeSystemName
        return stats;
    }
};

struct TradeSession
{
    std::string id;
    ContextType contextType;
    DateTime startDate;
    DateTime endDate;
    std::string parameterGroupId;
    TradeStatistics tradeStatistics;
    std::string tradeSystemName = GetTradeSystemName(); // Assign from function

    TradeSession() : id(UniqueIDGenerator::GenerateID()), startDate(DateTime()), endDate(DateTime()) {}
    TradeSession(DateTime startDate, DateTime endDate, TradeSystemParameterGroup tradeSystemParameterGroup, TradeStatistics tradeStatistics)
        : id(UniqueIDGenerator::GenerateID()), startDate(startDate), endDate(endDate), tradeStatistics(tradeStatistics), contextType(contextType) {}

    nlohmann::json ToJson() const
    {
        return {
            {"id", id},
            {"startDate", startDate.ToString()},
            {"endDate", endDate.ToString()},
            {"parameterGroupId", parameterGroupId},
            {"tradeStatistics", tradeStatistics.ToJson()},
            {"tradeSystemName", tradeSystemName}, // Add tradeSystemName to JSON
            {"contextType", contextType}};
    }

    static TradeSession FromJson(const nlohmann::json &j)
    {
        TradeSession result;
        result.id = j.at("id").get<std::string>();
        result.startDate = DateTime::FromString(j.at("startDate").get<std::string>());
        result.endDate = DateTime::FromString(j.at("endDate").get<std::string>());
        result.parameterGroupId = j.at("parameterGroupId").get<std::string>();
        result.tradeStatistics = TradeStatistics::FromJson(j.at("tradeStatistics"));
        result.tradeSystemName = j.at("tradeSystemName").get<std::string>(); // Add tradeSystemName
        return result;
    }
};

struct PositionData
{
    std::string id;
    std::string symbol;
    double positionQuantity = 0;
    double averagePrice = 0;
    double openProfitLoss = 0;
    double cumulativeProfitLoss = 0;
    int winTrades = 0;
    int loseTrades = 0;
    int totalTrades = 0;
    double maximumOpenPositionLoss = 0;
    double maximumOpenPositionProfit = 0;
    double lastTradeProfitLoss = 0;
    double positionQuantityWithAllWorkingOrders = 0;
    double positionQuantityWithExitWorkingOrders = 0;
    int workingOrdersExist = 0;
    double dailyProfitLoss = 0;
    DateTime lastFillDateTime;
    DateTime lastEntryDateTime;
    DateTime lastExitDateTime;
    double priceHighDuringPosition = 0;
    double dailyTotalQuantityFilled = 0;
    double priceLowDuringPosition = 0;
    double priorPositionQuantity = 0;
    double positionQuantityWithExitMarketOrders = 0;
    double totalQuantityFilled = 0;
    double lastTradeQuantity = 0;
    double tradeStatsDailyProfitLoss = 0;
    double tradeStatsDailyClosedQuantity = 0;
    double tradeStatsOpenProfitLoss = 0;
    double tradeStatsOpenQuantity = 0;
    int nonAttachedWorkingOrdersExist = 0;
    std::string tradeAccount;
    double positionQuantityWithMarketWorkingOrders = 0;
    double positionQuantityWithAllWorkingOrdersExceptNonMarketExits = 0;
    double positionQuantityWithAllWorkingOrdersExceptAllExits = 0;
    double allWorkingBuyOrdersQuantity = 0;
    double allWorkingSellOrdersQuantity = 0;
    std::string tradeSystemName = GetTradeSystemName(); // Assign from function

    PositionData() : id(UniqueIDGenerator::GenerateID()) {}

    nlohmann::json ToJson() const
    {
        return {
            {"id", id},
            {"symbol", symbol},
            {"positionQuantity", positionQuantity},
            {"averagePrice", averagePrice},
            {"openProfitLoss", openProfitLoss},
            {"cumulativeProfitLoss", cumulativeProfitLoss},
            {"winTrades", winTrades},
            {"loseTrades", loseTrades},
            {"totalTrades", totalTrades},
            {"maximumOpenPositionLoss", maximumOpenPositionLoss},
            {"maximumOpenPositionProfit", maximumOpenPositionProfit},
            {"lastTradeProfitLoss", lastTradeProfitLoss},
            {"positionQuantityWithAllWorkingOrders", positionQuantityWithAllWorkingOrders},
            {"positionQuantityWithExitWorkingOrders", positionQuantityWithExitWorkingOrders},
            {"workingOrdersExist", workingOrdersExist},
            {"dailyProfitLoss", dailyProfitLoss},
            {"lastFillDateTime", lastFillDateTime.ToString()},
            {"lastEntryDateTime", lastEntryDateTime.ToString()},
            {"lastExitDateTime", lastExitDateTime.ToString()},
            {"priceHighDuringPosition", priceHighDuringPosition},
            {"dailyTotalQuantityFilled", dailyTotalQuantityFilled},
            {"priceLowDuringPosition", priceLowDuringPosition},
            {"priorPositionQuantity", priorPositionQuantity},
            {"positionQuantityWithExitMarketOrders", positionQuantityWithExitMarketOrders},
            {"totalQuantityFilled", totalQuantityFilled},
            {"lastTradeQuantity", lastTradeQuantity},
            {"tradeStatsDailyProfitLoss", tradeStatsDailyProfitLoss},
            {"tradeStatsDailyClosedQuantity", tradeStatsDailyClosedQuantity},
            {"tradeStatsOpenProfitLoss", tradeStatsOpenProfitLoss},
            {"tradeStatsOpenQuantity", tradeStatsOpenQuantity},
            {"nonAttachedWorkingOrdersExist", nonAttachedWorkingOrdersExist},
            {"tradeAccount", tradeAccount},
            {"positionQuantityWithMarketWorkingOrders", positionQuantityWithMarketWorkingOrders},
            {"positionQuantityWithAllWorkingOrdersExceptNonMarketExits", positionQuantityWithAllWorkingOrdersExceptNonMarketExits},
            {"positionQuantityWithAllWorkingOrdersExceptAllExits", positionQuantityWithAllWorkingOrdersExceptAllExits},
            {"allWorkingBuyOrdersQuantity", allWorkingBuyOrdersQuantity},
            {"allWorkingSellOrdersQuantity", allWorkingSellOrdersQuantity},
            {"tradeSystemName", tradeSystemName} // Add tradeSystemName to JSON
        };
    }
};

struct DataBlob
{
    std::string id;
    std::string key;
    nlohmann::json data;
    std::string tradeSystemName = GetTradeSystemName(); // Assign from function

    DataBlob() : id(UniqueIDGenerator::GenerateID()) {}

    template <typename T>
    T MapToType() const
    {
        return data.get<T>();
    }

    nlohmann::json ToJson() const
    {
        return {
            {"id", id},
            {"key", key},
            {"data", data},
            {"tradeSystemName", tradeSystemName} // Add tradeSystemName to JSON
        };
    }
};

struct MachineLearningInput
{
    std::vector<TradeSystemParameter> parameters;
    std::vector<DataBlob> dataBlobs;
    std::string tradeSystemName = GetTradeSystemName(); // Assign from function

    nlohmann::json ToJson() const
    {
        nlohmann::json j;
        for (const auto &param : parameters)
        {
            j["parameters"].push_back(param.ToJson());
        }
        for (const auto &blob : dataBlobs)
        {
            j["dataBlobs"].push_back(blob.ToJson());
        }
        j["tradeSystemName"] = tradeSystemName; // Add tradeSystemName to JSON
        return j;
    }
};

// BaseLevel struct definition
struct BaseLevel
{
    std::string id;
    double price;
    std::string levelType;
    DateTime timestamp = DateTime();
    bool clearOnTouch = false;
    std::optional<double> targetPrice;
    std::optional<double> stopLossPrice;
    std::string tradeSystemName = GetTradeSystemName(); // Assign from function

    BaseLevel(double price, std::string levelType, DateTime timestamp)
        : id(UniqueIDGenerator::GenerateID()), price(price), levelType(levelType), timestamp(timestamp) {}

    virtual ~BaseLevel() = default;

    // Define the equality operator
    bool operator==(const BaseLevel &other) const
    {
        return price == other.price && levelType == other.levelType && timestamp.timePoint == other.timestamp.timePoint;
    }

    virtual nlohmann::json ToJson() const
    {
        nlohmann::json j;
        j["id"] = id;
        j["price"] = price;
        j["levelType"] = levelType;
        j["timestamp"] = timestamp.ToString();
        j["clearOnTouch"] = clearOnTouch;
        if (targetPrice.has_value())
            j["targetPrice"] = targetPrice.value();
        if (stopLossPrice.has_value())
            j["stopLossPrice"] = stopLossPrice.value();
        j["tradeSystemName"] = tradeSystemName; // Add tradeSystemName to JSON
        return j;
    }

    static BaseLevel FromJson(const nlohmann::json &j)
    {
        BaseLevel level(j.at("price").get<double>(), j.at("levelType").get<std::string>(), DateTime::FromString(j.at("timestamp").get<std::string>()));
        level.id = j.at("id").get<std::string>();
        level.clearOnTouch = j.at("extendUntilIntersection").get<bool>();
        if (j.contains("targetPrice"))
            level.targetPrice = j.at("targetPrice").get<double>();
        if (j.contains("stopLossPrice"))
            level.stopLossPrice = j.at("stopLossPrice").get<double>();
        level.tradeSystemName = j.at("tradeSystemName").get<std::string>(); // Add tradeSystemName
        return level;
    }

    virtual std::string ToString() const
    {
        std::ostringstream os;
        os << "BaseLevel [ID: " << id
           << ", Price: " << price
           << ", LevelType: " << levelType
           << ", Timestamp: " << timestamp.ToString()
           << ", ExtendUntilIntersection: " << clearOnTouch;
        if (targetPrice.has_value())
            os << ", TargetPrice: " << targetPrice.value();
        if (stopLossPrice.has_value())
            os << ", StopLossPrice: " << stopLossPrice.value();
        os << ", TradeSystemName: " << tradeSystemName; // Add tradeSystemName
        os << "]";
        return os.str();
    }
};

enum class TimeAndSalesType
{
    Marker,
    Bid,
    Ask,
    BidAskValues
};

struct TimeAndSales
{
    std::string id;
    DateTime dateTime;
    double price;
    int volume;
    double bid;
    double ask;
    int bidSize;
    int askSize;
    TimeAndSalesType type;
    std::string tradeSystemName = GetTradeSystemName(); // Assign from function

    TimeAndSales() : id(UniqueIDGenerator::GenerateID()) {}

    nlohmann::json ToJson() const
    {
        return {
            {"id", id},
            {"dateTime", dateTime.ToString()},
            {"price", price},
            {"volume", volume},
            {"bid", bid},
            {"ask", ask},
            {"bidSize", bidSize},
            {"askSize", askSize},
            {"type", static_cast<int>(type)},
            {"tradeSystemName", tradeSystemName} // Add tradeSystemName to JSON
        };
    }

    static TimeAndSales FromJson(const nlohmann::json &j)
    {
        TimeAndSales ts;
        ts.id = j.at("id").get<std::string>();
        ts.dateTime = DateTime::FromString(j.at("dateTime").get<std::string>());
        ts.price = j.at("price").get<double>();
        ts.volume = j.at("volume").get<int>();
        ts.bid = j.at("bid").get<double>();
        ts.ask = j.at("ask").get<double>();
        ts.bidSize = j.at("bidSize").get<int>();
        ts.askSize = j.at("askSize").get<int>();
        ts.type = static_cast<TimeAndSalesType>(j.at("type").get<int>());
        ts.tradeSystemName = j.at("tradeSystemName").get<std::string>(); // Add tradeSystemName
        return ts;
    }

    std::string ToString() const
    {
        std::ostringstream os;
        os << "TimeAndSales [ID: " << id
           << ", DateTime: " << dateTime.ToString()
           << ", Price: " << price
           << ", Volume: " << volume
           << ", Bid: " << bid
           << ", Ask: " << ask
           << ", BidSize: " << bidSize
           << ", AskSize: " << askSize
           << ", Type: " << static_cast<int>(type)
           << ", TradeSystemName: " << tradeSystemName; // Add tradeSystemName
        os << "]";
        return os.str();
    }
};

enum class MarketDepthType
{
    Bid,
    Ask
};

struct MarketDepth
{
    std::string id;
    DateTime timestamp;
    MarketDepthType type;
    float price;
    int quantity;
    int numOrders;
    float stackPullValue;
    std::string tradeSystemName = GetTradeSystemName(); // Assign from function

    MarketDepth() : id(UniqueIDGenerator::GenerateID()) {}

    nlohmann::json ToJson() const
    {
        return {
            {"id", id},
            {"timestamp", timestamp.ToString()},
            {"type", static_cast<int>(type)},
            {"price", price},
            {"quantity", quantity},
            {"numOrders", numOrders},
            {"stackPullValue", stackPullValue},
            {"tradeSystemName", tradeSystemName} // Add tradeSystemName to JSON
        };
    }

    static MarketDepth FromJson(const nlohmann::json &j)
    {
        MarketDepth md;
        md.id = j.at("id").get<std::string>();
        md.timestamp = DateTime::FromString(j.at("timestamp").get<std::string>());
        md.type = static_cast<MarketDepthType>(j.at("type").get<int>());
        md.price = j.at("price").get<float>();
        md.quantity = j.at("quantity").get<int>();
        md.numOrders = j.at("numOrders").get<int>();
        md.stackPullValue = j.at("stackPullValue").get<float>();
        md.tradeSystemName = j.at("tradeSystemName").get<std::string>(); // Add tradeSystemName
        return md;
    }

    std::string ToString() const
    {
        return "MarketDepth [ID: " + id +
               ", Timestamp: " + timestamp.ToString() +
               ", Type: " + std::to_string(static_cast<int>(type)) +
               ", Price: " + std::to_string(price) +
               ", Quantity: " + std::to_string(quantity) +
               ", NumOrders: " + std::to_string(numOrders) +
               ", StackPullValue: " + std::to_string(stackPullValue) +
               ", TradeSystemName: " + tradeSystemName + "]"; // Add tradeSystemName
    }
};

struct MarketOrderData
{
    std::string id;
    DateTime timestamp;
    double price;
    double quantity;
    uint32_t orderId;
    std::string tradeSystemName = GetTradeSystemName(); // Assign from function

    MarketOrderData() : id(UniqueIDGenerator::GenerateID()) {}

    nlohmann::json ToJson() const
    {
        return {
            {"id", id},
            {"timestamp", timestamp.ToString()},
            {"price", price},
            {"quantity", quantity},
            {"orderId", orderId},
            {"tradeSystemName", tradeSystemName} // Add tradeSystemName to JSON
        };
    }

    static MarketOrderData FromJson(const nlohmann::json &j)
    {
        MarketOrderData mod;
        mod.id = j.at("id").get<std::string>();
        mod.timestamp = DateTime::FromString(j.at("timestamp").get<std::string>());
        mod.price = j.at("price").get<double>();
        mod.quantity = j.at("quantity").get<double>();
        mod.orderId = j.at("orderId").get<uint32_t>();
        mod.tradeSystemName = j.at("tradeSystemName").get<std::string>(); // Add tradeSystemName
        return mod;
    }

    std::string ToString() const
    {
        return "MarketOrderData [ID: " + id +
               ", Timestamp: " + timestamp.ToString() +
               ", Price: " + std::to_string(price) +
               ", Quantity: " + std::to_string(quantity) +
               ", OrderID: " + std::to_string(orderId) +
               ", TradeSystemName: " + tradeSystemName + "]"; // Add tradeSystemName
    }
};

enum class UpdateIntervalType
{
    New_Bar,
    Always
};

struct TradingWindow
{
    DateTime startTime;
    DateTime endTime;

    // Getter for start time (returns DateTime object) with optional date
    DateTime GetStartTime(const DateTime *specificDate = nullptr) const
    {
        if (specificDate)
        {
            return CombineDateAndTime(*specificDate, startTime);
        }
        return CombineDateAndTime(DateTime(), startTime); // Use current date with start time
    }

    // Getter for end time (returns DateTime object) with optional date
    DateTime GetEndTime(const DateTime *specificDate = nullptr) const
    {
        if (specificDate)
        {
            return CombineDateAndTime(*specificDate, endTime);
        }
        return CombineDateAndTime(DateTime(), endTime); // Use current date with end time
    }

    nlohmann::json ToJson() const
    {
        return {
            {"startTime", GetStartTime().ToString()},
            {"endTime", GetEndTime().ToString()}};
    };

    static TradingWindow FromJson(const nlohmann::json &j)
    {
        TradingWindow tw;
        tw.startTime = DateTime::FromTimeString(j.at("startTime").get<std::string>());
        tw.endTime = DateTime::FromTimeString(j.at("endTime").get<std::string>());

        return tw;
    }

private:
    // Combines the date from `datePart` and the time from `timePart`
    DateTime CombineDateAndTime(const DateTime &datePart, const DateTime &timePart) const
    {
        // Get the time_t representations of both the date and time parts
        std::time_t dateT = datePart.ToTimeT();
        std::time_t timeT = timePart.ToTimeT();

        // Convert them to tm structures
        std::tm dateTm = *std::gmtime(&dateT);
        std::tm timeTm = *std::gmtime(&timeT);

        // Combine the date from dateTm and time from timeTm
        dateTm.tm_hour = timeTm.tm_hour;
        dateTm.tm_min = timeTm.tm_min;
        dateTm.tm_sec = 0;

        // Normalize the tm structure after adjustment
        std::time_t combinedTime = std::mktime(&dateTm);

        // Return the combined DateTime object
        return DateTime(std::chrono::system_clock::from_time_t(combinedTime));
    }
};

// SessionSettings structure
struct SessionSettings
{
    std::string barType;
    std::string barPeriod;
    UpdateIntervalType updateIntervalType;
    std::optional<TradingWindow> Monday;
    std::optional<TradingWindow> Tuesday;
    std::optional<TradingWindow> Wednesday;
    std::optional<TradingWindow> Thursday;
    std::optional<TradingWindow> Friday;
    std::optional<TradingWindow> Saturday;
    std::optional<TradingWindow> Sunday;

    nlohmann::json ToJson() const
    {
        nlohmann::json tradingWindowJson;

        if (Monday)
            tradingWindowJson["Monday"] = Monday->ToJson();
        if (Tuesday)
            tradingWindowJson["Tuesday"] = Tuesday->ToJson();
        if (Wednesday)
            tradingWindowJson["Wednesday"] = Wednesday->ToJson();
        if (Thursday)
            tradingWindowJson["Thursday"] = Thursday->ToJson();
        if (Friday)
            tradingWindowJson["Friday"] = Friday->ToJson();
        if (Saturday)
            tradingWindowJson["Saturday"] = Saturday->ToJson();
        if (Sunday)
            tradingWindowJson["Sunday"] = Sunday->ToJson();

        return {
            {"barType", barType},
            {"barPeriod", barPeriod},
            {"updateIntervalType", static_cast<int>(updateIntervalType)}, // Serialize the enum as an integer
            {"tradingWindow", tradingWindowJson}                          // Nested tradingWindow object
        };
    }

    static SessionSettings FromJson(const nlohmann::json &j)
    {
        SessionSettings ss;
        ss.barType = j.at("barType").get<std::string>();
        ss.barPeriod = j.at("barPeriod").get<std::string>();

        int updateInterval = j.at("updateIntervalType").get<int>();
        ss.updateIntervalType = static_cast<UpdateIntervalType>(updateInterval); // Convert int to enum

        // Checking if "tradingWindow" exists and mapping weekdays correctly
        if (j.contains("tradingWindow"))
        {
            const auto &tradingWindow = j.at("tradingWindow");

            if (tradingWindow.contains("Monday") && !tradingWindow["Monday"].is_null())
                ss.Monday = TradingWindow::FromJson(tradingWindow.at("Monday"));
            if (tradingWindow.contains("Tuesday") && !tradingWindow["Tuesday"].is_null())
                ss.Tuesday = TradingWindow::FromJson(tradingWindow.at("Tuesday"));
            if (tradingWindow.contains("Wednesday") && !tradingWindow["Wednesday"].is_null())
                ss.Wednesday = TradingWindow::FromJson(tradingWindow.at("Wednesday"));
            if (tradingWindow.contains("Thursday") && !tradingWindow["Thursday"].is_null())
                ss.Thursday = TradingWindow::FromJson(tradingWindow.at("Thursday"));
            if (tradingWindow.contains("Friday") && !tradingWindow["Friday"].is_null())
                ss.Friday = TradingWindow::FromJson(tradingWindow.at("Friday"));
            if (tradingWindow.contains("Saturday") && !tradingWindow["Saturday"].is_null())
                ss.Saturday = TradingWindow::FromJson(tradingWindow.at("Saturday"));
            if (tradingWindow.contains("Sunday") && !tradingWindow["Sunday"].is_null())
                ss.Sunday = TradingWindow::FromJson(tradingWindow.at("Sunday"));
        }

        return ss;
    }
};

// SystemSettings structure
struct SystemSettings
{
    bool enableLogging;
    int liveResultsSnapshotIntervalMinutes;

    nlohmann::json ToJson() const
    {
        return {
            {"enableLogging", enableLogging},
            {"liveResultsSnapshotIntervalMinutes", liveResultsSnapshotIntervalMinutes}};
    }

    static SystemSettings FromJson(const nlohmann::json &j)
    {
        SystemSettings ss;
        ss.enableLogging = j.at("enableLogging").get<bool>();
        ss.liveResultsSnapshotIntervalMinutes = j.at("liveResultsSnapshotIntervalMinutes").get<int>();
        return ss;
    }
};

// DataProcessingServer structure
struct DataProcessingServer
{
    std::string host;
    int port;

    nlohmann::json ToJson() const
    {
        return {
            {"host", host},
            {"port", port}};
    }

    static DataProcessingServer FromJson(const nlohmann::json &j)
    {
        DataProcessingServer dps;
        dps.host = j.at("host").get<std::string>();
        dps.port = j.at("port").get<int>();
        return dps;
    }
};

struct TradingSystem
{
    std::string name;
    std::optional<std::string> description;
    std::optional<SessionSettings> sessionSettings;
    std::optional<SystemSettings> systemSettings;

    nlohmann::json ToJson() const
    {
        nlohmann::json j = {{"name", name}};
        if (description)
            j["description"] = *description;
        if (sessionSettings)
            j["sessionSettings"] = sessionSettings->ToJson();
        if (systemSettings)
            j["systemSettings"] = systemSettings->ToJson();
        return j;
    }

    static TradingSystem FromJson(const nlohmann::json &j)
    {
        TradingSystem ts;
        ts.name = j.at("name").get<std::string>();
        if (j.contains("description"))
            ts.description = j.at("description").get<std::string>();
        if (j.contains("sessionSettings"))
            ts.sessionSettings = SessionSettings::FromJson(j.at("sessionSettings"));
        if (j.contains("systemSettings"))
            ts.systemSettings = SystemSettings::FromJson(j.at("systemSettings"));
        return ts;
    }

    // Helper Function to Get the Trading Window for a Specific Day
    std::optional<TradingWindow> GetTradingWindow(const std::string &dayOfWeek) const
    {
        if (sessionSettings)
        {
            if (dayOfWeek == "Monday" && sessionSettings->Monday)
            {
                return sessionSettings->Monday;
            }
            else if (dayOfWeek == "Tuesday" && sessionSettings->Tuesday)
            {
                return sessionSettings->Tuesday;
            }
            else if (dayOfWeek == "Wednesday" && sessionSettings->Wednesday)
            {
                return sessionSettings->Wednesday;
            }
            else if (dayOfWeek == "Thursday" && sessionSettings->Thursday)
            {
                return sessionSettings->Thursday;
            }
            else if (dayOfWeek == "Friday" && sessionSettings->Friday)
            {
                return sessionSettings->Friday;
            }
            else if (dayOfWeek == "Saturday" && sessionSettings->Saturday)
            {
                return sessionSettings->Saturday;
            }
            else if (dayOfWeek == "Sunday" && sessionSettings->Sunday)
            {
                return sessionSettings->Sunday;
            }
        }
        return std::nullopt;
    }

    // Helper Function to Get the Bar Type and Period
    std::optional<std::pair<std::string, std::string>> GetBarTypeAndPeriod() const
    {
        if (sessionSettings)
        {
            return std::make_pair(sessionSettings->barType, sessionSettings->barPeriod);
        }
        return std::nullopt;
    }

    // Helper Function to Get the Update Interval Type
    std::optional<UpdateIntervalType> GetUpdateIntervalType() const
    {
        if (sessionSettings)
        {
            return sessionSettings->updateIntervalType;
        }
        return std::nullopt;
    }
    // Helper Function to Check if Logging is Enabled
    bool IsLoggingEnabled() const
    {
        return systemSettings ? systemSettings->enableLogging : false;
    }

    // Helper Function to Get the Live Results Snapshot Interval
    std::optional<int> GetLiveResultsSnapshotInterval() const
    {
        if (systemSettings)
        {
            return systemSettings->liveResultsSnapshotIntervalMinutes;
        }
        return std::nullopt;
    }
};
namespace nlohmann
{
    template <>
    struct adl_serializer<TradingSystem>
    {
        static void to_json(json &j, const TradingSystem &ts)
        {
            j = ts.ToJson();
        }

        static void from_json(const json &j, TradingSystem &ts)
        {
            ts = TradingSystem::FromJson(j);
        }
    };
};

#endif // COMMONTYPES_H
