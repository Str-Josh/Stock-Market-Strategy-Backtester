//
// Author: Joshua Carter
//

#include <iostream>
#include <map>
#include <vector>

#include "Stock.h"
#include "Position.h"
#include "Portfolio.h"

// #include "../CustomLibs/Interpolation/DividedDifference.h"

#include "../lib/json/single_include/nlohmann/json.hpp"


int main() {
    // Stock class instance.
    STOCK_H::Stock stock_CPRX = Stock("CPRX");
    stock_CPRX.getHistoricalStockData("HistoricalData/HistoricalData_Year_CPRX.csv");
    stock_CPRX.calculateTechnicalIndicators();

    // Position class instance.
    POSITION_H::Position<Stock> CPRX_Position = Position<Stock>(&stock_CPRX);

    // Portfolio class instance.
    PORTFOLIO_H::Portfolio portfolio;
    portfolio.addPosition(CPRX_Position);

    bool buySignalTrend = false;
    bool sellSignalTrend = false;

    unsigned short int signalTrend;  // 0/1 --> buy/sell

    // The # of shares to buy/sell on signal
    unsigned short int sharesToBuySell = 100;  // Let's use 100 for default before making more dynamic with other calculations.

    unsigned short int sharesBought = 0;
    unsigned short int sharesSold = 0;

    std::vector<double> profitLossData;  // store the profit/loss of each day.

    unsigned short int _counter = 0;
    for (auto element : stock_CPRX.stockHistoricalDataDeque) {
        auto technicalIndicators = stock_CPRX.stockTechnicalIndicatorsMap[element.date];
        /// This is the Strategy implementation. Determine when to throw buy/sell signals using different technical indicators.
        // Currently using the MACD but will add more dynamic behavior for the strategy to implement at a later date.
        
        // Determines if the start of the sequence is in a buy or sell signal.
        if (_counter == 0) {
            if (technicalIndicators.movingAverageConvergenceDivergence > technicalIndicators.movingAverageConvergenceDivergenceSignal) {
                // Throw buy signal
                CPRX_Position.buy(element.date, element.priceClose, sharesToBuySell);
                sharesBought += sharesToBuySell;
                // std::cout << "BUY: " << element.date << "\n";
                // buySignalTrend = true;  // Tells the algo that we're curr in a position with bought so for now can't buy more
                signalTrend = 0;
            }
            else if (technicalIndicators.movingAverageConvergenceDivergence < technicalIndicators.movingAverageConvergenceDivergenceSignal) {
                // Throw sell signal
                /// CPRX_Position.sell(element.date, element.priceClose, 100);
                int currentShares_CPRX = CPRX_Position.getNumberShares();
                if (currentShares_CPRX > 0) {
                    CPRX_Position.sell(element.date, element.priceClose, currentShares_CPRX);
                    profitLossData.push_back(CPRX_Position.getProfitLoss());
                }
                else { }  // Can't sell something you don't have.
                
                std::cout << "SELL: " << element.date << "\n";
                /// sellSignalTrend = true;
                signalTrend = 1;
            }
            else { std::cout << "MACD == MACD Signal.\n"; }

            // std::cout << "Start Date:  " << element.date << "\n" << "Signal Trend:  " << signalTrend << "\n";
            _counter += 1;
        }

        // Determine what to do with the particular trend... Either a buy trend (0) or sell trend (1).
        if (signalTrend == 1) {
            // We're looking for a change in trend.
            if (technicalIndicators.movingAverageConvergenceDivergence > technicalIndicators.movingAverageConvergenceDivergenceSignal) {
                // Send buy signal
                // std::cout << "BUY:  " << element.date << "\n";
                CPRX_Position.buy(element.date, element.priceClose, sharesToBuySell);
                sharesBought += sharesToBuySell;
                signalTrend = 0;
                ///buySignalTrend = true;
                ///sellSignalTrend = false;
                // std::cout << "Profit/Loss:  " << CPRX_Position.getProfitLoss() << "\n\n";
            }
        }
        else if (signalTrend == 0) {  // if (buySignalTrend) {
            if (technicalIndicators.movingAverageConvergenceDivergence < technicalIndicators.movingAverageConvergenceDivergenceSignal) {
                // Send sell signal
                int currentShares_CPRX = CPRX_Position.getNumberShares();
                if (currentShares_CPRX > 0) {
                    /// When we have enough shares so that we can sell.
                    // std::cout << "SELL: " << element.date << "\n";
                    /// CPRX_Position.sell(element.date, element.priceClose, currentShares_CPRX);
                    CPRX_Position.sell(element.date, element.priceClose, sharesToBuySell);
                    profitLossData.push_back(CPRX_Position.getProfitLoss());  // since PL only changes when selling shares.
                }
                else {
                    /// in the event there aren't any shares, we have to wait to buy.
                }
                signalTrend = 1;
                /// sellSignalTrend = true;
                /// buySignalTrend = false;
                // std::cout << "Profit/Loss:  " << CPRX_Position.getProfitLoss() << "\n\n";
            }
        }
        _counter += 1;
    }

    // Calculate the average of profit/loss
    double avgPL = 0;
    for (auto j = 0; j < profitLossData.size(); j++) { avgPL += profitLossData.at(j); }
    avgPL /= profitLossData.size();
    std::cout << "Average Profit/Loss:  " << avgPL << "\n";

    std::cout << "Program executed successfully\n>> ";
    std::cin.get();
    return 1;
}
