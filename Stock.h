//
// Author: Joshua Carter
// Date: 08/12/2024
//

#ifndef STOCK_H
#define STOCK_H

#include <winsock2.h>
#include <filesystem>
#include <locale>
#include <ctime>
#include <deque>
#include <queue>
#include <tuple>
#include <sstream>
#include <fstream>
#include <iostream>
#include <optional>
#include <map>
#include <sstream> // std::stringstream
#include <stdexcept> // std::runtime_error
#include <string>
#include <utility> // std::pair
#include <vector>

#include "apiRequestBuilder.h"

#include "../lib/json/single_include/nlohmann/json.hpp"


/// @brief Methods to analyze stock data.
class Stock {
    private:
        // Standard information for the stock company.
        std::string tickerSymbol;
        std::string stockExchange;
        std::string industry;
        std::string sector;
        std::string historicalStockDataFilename;

        // Information for API request data call.
        std::string timeInterval = "daily";

        // Information to be used in various calculations but might not be useful elsewhere.
        double total_sum = 0.0;
        double benchmarkRate = 0.0;
        double adjustedBenchmarkRate;
        double maximumClosePrice;

        // R Squared Calculations.
        long double meanX = 0.0;
        long double meanY = 0.0;
        long double meanXY = 0.0;
        long double meanX2 = 0.0;
        long double meanY2 = 0.0;

        /// @brief Get Technical Indicator data from Alpha Vantage Web API.
        /// @param apiFunctionType The technical indicator.
        /// @param timePeriod Number of data points used to calculate each technical indicator value.
        /// @param customQueryParameters Custom query parameters for custom API request.
        /// @return JSON data received from the Web API.
        nlohmann::json getApiData(std::string apiFunctionType, unsigned int timePeriod, std::map<std::string, std::string> customQueryParameters = {});

    public:
        // Drawdown evaluators for performance metrics.
        /*
        std::function<double(double, double)> dollarDrawdown = [](double price, double peak) { return peak - price; };
        std::function<double(double, double)> percentDrawdown = [](double price, double peak) { return -((price / peak) - 1); };
        std::function<double(double, double)> logDrawdown = [](double price, double peak) { return std::log(peak / price); };
        std::map<std::string, std::function<double(double, double)>> drawdownEvaluators = { {"dollar", dollarDrawdown}, {"percent", percentDrawdown}, {"log", logDrawdown} };
        */
        std::map<std::string, std::function<double(double, double)>> drawdownEvaluatorsMap = {
            {"dollar", [](double price, double peak) { return peak - price; }},
            {"percent", [](double price, double peak) { return -((price / peak) - 1); }},
            {"log", [](double price, double peak) { return std::log(peak / price); }}
        };

        struct performanceMetricsStruct {
            double annualizedVolatility;
            double sharpeRatio;
            double CAGR;
            double sortinoRatio;

            double annualizedDownsideDeviation;
            double logMaximumDrawdown;
            double percentMaximumDrawdown;
            double logMaxDrawdownRatio;
            double calmarRatio;
            double pureProfitScore;
            double jensensAlpha;
        };

        struct technicalIndicators {
            /// @brief .
            double movingAverageDays20;

            /// @brief .
            double movingAverageDays50;

            /// @brief .
            double movingAverageDays200;

            /// @brief .
            double movingAverageConvergenceDivergence;

            /// @brief .
            double movingAverageConvergenceDivergenceSignal;

            /// @brief .
            double relativeStrengthIndex;

            /// @brief .
            double rateOfChange;
            
            /// @brief Identifies support and resistance levels
            double ichimokuCloud;

            /// @brief Identify when new trend is set to begin
            double aroonUp;

            /// @brief .
            double aroonDown;

            /// @brief .
            double aroonOscillator;

            /// @brief .
            double onBalanceVolume;

            /// @brief the CMO divides the total movement by the net movement ((up - down) / (up + down)).
            /*
            There are several ways to interpret the CMO. 
            Values over 50 -> overbought conditions; values under -50 -> oversold conditions. 
            High CMO values indicate strong trends. 
            When the CMO crosses above a moving average of the CMO, it is a buy signal, crossing down is a sell signal.
            */
            double chandeMomentumOscillator;

            /// @brief .
            double plusDI;

            /// @brief .
            double minusDI;

            /// @brief DX is usually smoothed with a moving average (i.e. the ADX). 
            /*
            The values range from 0 to 100, but rarely get above 60. 
            To interpret the DX, consider a high number to be a strong trend, and a low number, a weak trend.
            */
            double DX;

            /// @brief Variable-term RSI. Interpreted same as RSI but provides signals sooner.
            double DMI;

            /// @brief EMV emphasizes days in which the stock is moving easily and minimizes the days in which the stock is finding it difficult to move. 
            /*
            This indicator is used frequently with equivolume charts to identify market formations. 
            A buy signal is generated when the EMV crosses above zero, a sell signal when it crosses below zero. 
            When the EMV hovers around zero, then there are small price movements and/or high volume, which is to say, the price is not moving easily.
            
            The volume is divided by a volume increment (typically 10,000) 
            to make the resultant numbers larger and easier to work with. 
            The EMV is usually smoothed with a moving average.
            */
            double easeOfMovement;

            /// @brief Similar to RSI, but uses the movement between the open and close whereas the RSI uses the movement between the close and the previous close. 
            /*
            IMI values over 70 indicate an overbought condition, and values under 30 indicate oversold.
            */
            double intradayMomentumIndex;

            /// @brief The MFI measures the price movement per unit of volume.
            /*
            To interpret the MFI, compare it to the volume. 
            When the MFI is high and volume is low, it signals a fake trend which will soon reverse. 
            When the MFI is low and volume is high, it signals a new trend in either direction is about to occur. 
            When the MFI is low and volume is also low, it signals a fading market and an impending trend reversal. 
            When the MFI is high and volume is also high, it signals a strong trend.
            */
            double marketFacilitationIndex;

            /// @brief Measurement of acceleration & deceleration of prices. 
            /*
            It indicates if prices are increasing at an increasing rate or decreasing at a decreasing rate. 
            The Momentum function can be applied to the price, or to any other data series.
            */
            double momentum;

            /// @brief Calculates ratio of money flowing into and out of a security. 
            /*
            To interpret the Money Flow Index, look for divergence with price to signal reversals. 
            Money Flow Index values range from 0 to 100. Values above 80/below 20 indicate market tops/bottoms.
            */
            double moneyFlowIndex;
        };

        /*
        /// @brief Get Technical Indicator data from Alpha Vantage Web API.
        /// @param apiFunctionType The technical indicator.
        /// @param timePeriod Number of data points used to calculate each technical indicator value.
        /// @param customQueryParameters Custom query parameters for custom API request.
        /// @return JSON data received from the Web API.
        nlohmann::json getApiData(std::string apiFunctionType, unsigned int timePeriod, std::map<std::string, std::string> customQueryParameters = {});
        */

        struct historicalDataStruct {
            std::string date;
            double priceOpen;
            double priceClose;
            double high;
            double low;
            double adjustedClosePrice;
            int volume;
            // Below are calculated data.
            double returns;
            double percentReturns;
            double logReturns;
            double downside;
            double logDrawdown;
            double percentDrawdown;
            double cumulativeMax;

            // Other things
            double averagePrice;
            double medianPrice;
            double typicalPrice;
            double moneyFlow;
        };

        performanceMetricsStruct performanceMetrics;
        std::deque<historicalDataStruct> stockHistoricalDataDeque;
        std::map<std::string, technicalIndicators> stockTechnicalIndicatorsMap;

        // Stock();

        /// @brief Constructor for the Stock class.
        /// @param tickerSymbol The universal symbol to represent a given financial instrument.
        Stock(const std::string tickerSymbol);
        
        const Stock* benchmarkStock = nullptr;

        // Getters & Setters

        std::string getTickerSymbol();

        void setStockExchange(std::string stockExchange);
        std::string getStockExchange();

        void setStockSector(std::string stockSector);
        std::string getStockSector();

        void setBenchmarkStock(const Stock* stock);
        const Stock* getBenchmarkStock() const;


        // Analysis metrics calculation methods.

        void calculatePerformanceMetrics();
        
        /// @brief Create or load JSON data for technical indicators.
        void calculateTechnicalIndicators();

        /// @brief Retrieve historical data from CSV file.
        /// @param filename the CSV file location for the historical data for the ticker symbol.
        /// @return A deque of structs for the historical data.
        std::deque<historicalDataStruct> getHistoricalStockData(std::string filename);
};

#endif