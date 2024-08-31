#ifndef CALCULATIONS_H
#define CALCULATIONS_H

#include <vector>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <stdexcept>
#include <tuple>
#include <optional>
#include "CommonTypes.h"

namespace Calculations {

    // Utility Functions
    /**
     * @brief Computes the sum of the last `length` elements of `data`.
     * @param data A vector of float values.
     * @param length Number of elements to include in the sum.
     * @return The sum of the specified elements.
     * @throws std::invalid_argument If `length` is less than or equal to zero or greater than the size of `data`.
     * 
     * @details This function can be used in scenarios where you need to calculate the total value of a series of data points, such as the total volume traded over a specified period or the total returns over a series of days.
     */
    static inline float Sum(const std::vector<float>& data, int length) {
        if (length <= 0 || length > data.size()) {
            throw std::invalid_argument("Invalid length for summation.");
        }
        return std::accumulate(data.end() - length, data.end(), 0.0f);
    }

    /**
     * @brief Computes the mean (average) of the last `length` elements of `data`.
     * @param data A vector of float values.
     * @param length Number of elements to include in the mean calculation.
     * @return The mean of the specified elements.
     * @throws std::invalid_argument If `length` is less than or equal to zero or greater than the size of `data`.
     * 
     * @details The mean can be used to determine the average price, return, or volume over a specified period, which is useful for identifying trends and making informed trading decisions.
     */
    static inline float Mean(const std::vector<float>& data, int length) {
        return Sum(data, length) / length;
    }

    /**
     * @brief Computes the variance of the last `length` elements of `data`.
     * @param data A vector of float values.
     * @param length Number of elements to include in the variance calculation.
     * @return The variance of the specified elements.
     * @throws std::invalid_argument If `length` is less than or equal to zero or greater than the size of `data`.
     * 
     * @details Variance is a measure of the dispersion of data points around the mean. It is useful for assessing the volatility of an asset's price, helping traders understand the risk associated with the asset.
     */
    static inline float Variance(const std::vector<float>& data, int length) {
        float mean = Mean(data, length);
        float variance = 0.0f;
        for (size_t i = data.size() - length; i < data.size(); ++i) {
            variance += std::pow(data[i] - mean, 2.0f);
        }
        return variance / length;
    }

    /**
     * @brief Computes the standard deviation of the last `length` elements of `data`.
     * @param data A vector of float values.
     * @param length Number of elements to include in the standard deviation calculation.
     * @return The standard deviation of the specified elements.
     * @throws std::invalid_argument If `length` is less than or equal to zero or greater than the size of `data`.
     * 
     * @details Standard deviation is a widely used measure of volatility. It helps traders quantify the amount of variation or dispersion in a set of price data, which is crucial for risk management and option pricing.
     */
    static inline float StandardDeviation(const std::vector<float>& data, int length) {
        return std::sqrt(Variance(data, length));
    }

    // Moving Averages
    /**
     * @brief Computes the Simple Moving Average (SMA) of the last `length` elements of `data`.
     * @param data A vector of float values.
     * @param length Number of elements to include in the SMA calculation.
     * @return The SMA of the specified elements.
     * @throws std::invalid_argument If `length` is less than or equal to zero or greater than the size of `data`.
     * 
     * @details The SMA is used to smooth out price data to identify trends over a specified period. It is useful in technical analysis to determine support and resistance levels.
     */
    static inline float CalculateSMA(const std::vector<float>& data, int length) {
        return Mean(data, length);
    }

    /**
     * @brief Computes the Exponential Moving Average (EMA) of the last `length` elements of `data`.
     * @param data A vector of float values.
     * @param length Number of elements to include in the EMA calculation.
     * @return The EMA of the specified elements.
     * @throws std::invalid_argument If `length` is less than or equal to zero or greater than the size of `data`.
     * 
     * @details The EMA gives more weight to recent prices, making it more responsive to new information. It is commonly used in technical analysis to identify trends and reversals.
     */
    static inline float CalculateEMA(const std::vector<float>& data, int length) {
        if (length <= 0 || length > data.size()) {
            throw std::invalid_argument("Invalid length for EMA calculation.");
        }
        float multiplier = 2.0f / (length + 1);
        float ema = data[data.size() - length];
        for (size_t i = data.size() - length + 1; i < data.size(); ++i) {
            ema = ((data[i] - ema) * multiplier) + ema;
        }
        return ema;
    }

    // Volatility and Momentum
    /**
     * @brief Computes the Relative Strength Index (RSI) based on the provided gains and losses.
     * @param gains A vector of gains.
     * @param losses A vector of losses.
     * @param length The period length for the RSI calculation.
     * @return The RSI value.
     * @throws std::invalid_argument If `gains` and `losses` are of different sizes or if `length` is invalid.
     * 
     * @details The RSI is a momentum oscillator that measures the speed and change of price movements. It is used to identify overbought or oversold conditions in a market.
     */
    static inline float CalculateRSI(const std::vector<float>& gains, const std::vector<float>& losses, int length) {
        if (gains.size() != losses.size() || length <= 0 || length > gains.size()) {
            throw std::invalid_argument("Invalid length or data size for RSI calculation.");
        }
        float avg_gain = Mean(gains, length);
        float avg_loss = Mean(losses, length);
        float rs = avg_gain / avg_loss;
        return 100 - (100 / (1 + rs));
    }

    /**
     * @brief Computes the Bollinger Bands (upper band, middle band, lower band).
     * @param data A vector of float values.
     * @param length The period length for the SMA calculation.
     * @param num_std_dev The number of standard deviations for the bands.
     * @return A tuple containing the upper band, middle band (SMA), and lower band values.
     * @throws std::invalid_argument If `length` is invalid.
     * 
     * @details Bollinger Bands are used to measure market volatility. They consist of a middle band (SMA) and two outer bands (standard deviations above and below the SMA). These bands can indicate potential overbought or oversold conditions.
     */
    static inline std::tuple<float, float, float> CalculateBollingerBands(const std::vector<float>& data, int length, float num_std_dev) {
        float sma = CalculateSMA(data, length);
        float std_dev = StandardDeviation(data, length);
        float upper_band = sma + num_std_dev * std_dev;
        float lower_band = sma - num_std_dev * std_dev;
        return std::make_tuple(upper_band, sma, lower_band);
    }

    /**
     * @brief Computes the Moving Average Convergence Divergence (MACD) and its signal line.
     * @param data A vector of float values.
     * @param short_length The period length for the short EMA.
     * @param long_length The period length for the long EMA.
     * @param signal_length The period length for the signal line EMA.
     * @return A pair containing the MACD value and the signal line value.
     * @throws std::invalid_argument If the period lengths are invalid.
     * 
     * @details The MACD is used to identify changes in the strength, direction, momentum, and duration of a trend. It is a trend-following momentum indicator that shows the relationship between two moving averages of a security’s price.
     */
    static inline std::pair<float, float> CalculateMACD(const std::vector<float>& data, int short_length, int long_length, int signal_length) {
        if (short_length <= 0 || long_length <= 0 || signal_length <= 0 || data.size() < long_length) {
            throw std::invalid_argument("Invalid lengths for MACD calculation.");
        }
        float short_ema = CalculateEMA(data, short_length);
        float long_ema = CalculateEMA(data, long_length);
        float macd = short_ema - long_ema;
        std::vector<float> macd_line(data.size() - long_length + 1);
        for (size_t i = long_length - 1; i < data.size(); ++i) {
            macd_line[i - long_length + 1] = CalculateEMA(std::vector<float>(data.begin() + i - long_length + 1, data.begin() + i + 1), short_length) - CalculateEMA(std::vector<float>(data.begin() + i - long_length + 1, data.begin() + i + 1), long_length);
        }
        float signal_line = CalculateEMA(macd_line, signal_length);
        return std::make_pair(macd, signal_line);
    }

    // Bar-based Calculations
    /**
     * @brief Calculates the OHLC average of a bar.
     * @param bar A BarData structure containing the OHLC values.
     * @return The OHLC average.
     * 
     * @details The OHLC average is used to provide a single representative value for a bar in technical analysis. It can be useful for smoothing data and calculating moving averages.
     */
    static inline double CalculateOHLCAvg(const BarData& bar) {
        return (bar.open + bar.high + bar.low + bar.close) / 4.0;
    }

    /**
     * @brief Calculates the HLC average of a bar.
     * @param bar A BarData structure containing the HLC values.
     * @return The HLC average.
     * 
     * @details The HLC average provides a simpler representation of price data that can be used for various technical indicators. It can help in smoothing out the price data for better trend analysis.
     */
    static inline double CalculateHLCAvg(const BarData& bar) {
        return (bar.high + bar.low + bar.close) / 3.0;
    }

    /**
     * @brief Calculates the HL average of a bar.
     * @param bar A BarData structure containing the HL values.
     * @return The HL average.
     * 
     * @details The HL average is a basic calculation used in various technical indicators and provides a simple measure of central tendency in price data.
     */
    static inline double CalculateHLAverage(const BarData& bar) {
        return (bar.high + bar.low) / 2.0;
    }

    /**
     * @brief Calculates the volume-weighted average price (VWAP) for a vector of BarData.
     * @param bars A vector of BarData structures.
     * @return The VWAP value.
     * 
     * @details VWAP is used to measure the average price a security has traded at throughout the day, based on both volume and price. It is a trading benchmark that gives traders insight into both the trend and value of a security.
     */
    static inline double CalculateVWAP(const std::vector<BarData>& bars) {
        double cumulativeTotalPriceVolume = 0.0;
        double cumulativeVolume = 0.0;
        for (const auto& bar : bars) {
            double avgPrice = CalculateOHLCAvg(bar);
            double volume = bar.volume.value_or(0.0);
            cumulativeTotalPriceVolume += avgPrice * volume;
            cumulativeVolume += volume;
        }
        return cumulativeTotalPriceVolume / cumulativeVolume;
    }

    // Futures-specific Calculations
    /**
     * @brief Computes the cost of carry for a futures contract.
     * @param spot_price The current spot price of the underlying asset.
     * @param rate The risk-free interest rate.
     * @param time_to_maturity The time to maturity of the futures contract (in years).
     * @param storage_cost The cost of storing the underlying asset (as a percentage).
     * @param convenience_yield The convenience yield of the underlying asset (as a percentage).
     * @return The cost of carry.
     * 
     * @details The cost of carry is used in futures pricing and includes costs like storage and interest forgone on the capital tied up in the asset. It helps in understanding the fair value of a futures contract.
     */
    static inline double CalculateCostOfCarry(double spot_price, double rate, double time_to_maturity, double storage_cost, double convenience_yield) {
        return spot_price * std::exp((rate + storage_cost - convenience_yield) * time_to_maturity);
    }

    /**
     * @brief Computes the futures price using the cost of carry model.
     * @param spot_price The current spot price of the underlying asset.
     * @param rate The risk-free interest rate.
     * @param time_to_maturity The time to maturity of the futures contract (in years).
     * @param storage_cost The cost of storing the underlying asset (as a percentage).
     * @param convenience_yield The convenience yield of the underlying asset (as a percentage).
     * @return The futures price.
     * 
     * @details The futures price calculation helps traders determine the fair value of a futures contract by accounting for the cost of carry. It is crucial for pricing and arbitrage strategies.
     */
    static inline double CalculateFuturesPrice(double spot_price, double rate, double time_to_maturity, double storage_cost, double convenience_yield) {
        return spot_price * std::exp((rate + storage_cost - convenience_yield) * time_to_maturity);
    }

    /**
     * @brief Computes the initial margin requirement for a futures contract.
     * @param contract_value The notional value of the futures contract.
     * @param margin_rate The initial margin rate (as a percentage).
     * @return The initial margin requirement.
     * 
     * @details Initial margin is the upfront payment required to enter into a futures position. This calculation helps traders understand their capital requirements for opening positions.
     */
    static inline double CalculateInitialMargin(double contract_value, double margin_rate) {
        return contract_value * margin_rate;
    }

    /**
     * @brief Computes the maintenance margin requirement for a futures contract.
     * @param contract_value The notional value of the futures contract.
     * @param maintenance_margin_rate The maintenance margin rate (as a percentage).
     * @return The maintenance margin requirement.
     * 
     * @details Maintenance margin is the minimum account balance required to keep a futures position open. This calculation is important for risk management and ensuring sufficient capital is available to maintain positions.
     */
    static inline double CalculateMaintenanceMargin(double contract_value, double maintenance_margin_rate) {
        return contract_value * maintenance_margin_rate;
    }

} // namespace Calculations

#endif // CALCULATIONS_H
