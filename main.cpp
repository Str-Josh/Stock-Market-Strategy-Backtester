//
// Author: Joshua Carter
//

#include <iostream>
#include <map>
#include <vector>

#include "Stock.h"
#include "Position.h"
#include "Portfolio.h"

#include "../CustomLibs/Interpolation/DividedDifference.h"

#include "../lib/json/single_include/nlohmann/json.hpp"


// void testBacktestSystem() {
//     STOCK_H::Stock stock_CPRX = Stock("CPRX");
//     stock_CPRX.getHistoricalStockData("HistoricalData/HistoricalData_Year_CPRX.csv");
//     std::cout << stock_CPRX.getTickerSymbol() << "\n";
//     stock_CPRX.calculateTechnicalIndicators();
//     POSITION_H::Position<Stock> CPRX_Position = Position<Stock>();
//     PORTFOLIO_H::Portfolio portfolio;
//     portfolio.addPosition(CPRX_Position);
//     std::cout << "Program executed successfully\n>> ";
// }

/*
void simulation() {
    STOCK_H::Stock stock_CPRX = Stock("CPRX");
    stock_CPRX.getHistoricalStockData("HistoricalData/HistoricalData_Year_CPRX.csv");
    // std::cout << stock_CPRX.getTickerSymbol() << "\n";
    stock_CPRX.calculateTechnicalIndicators();
    POSITION_H::Position<Stock> CPRX_Position = Position<Stock>();
    CPRX_Position.buy("2023-08-21", 14.6, 100);
    CPRX_Position.sell("2023-09-01", 13.92, 20);
    if (element.date == "2023-08-21") {
        CPRX_Position.buy(element.date, element.priceClose, 100);
    }

    if (element.date == "2023-09-01") {
        CPRX_Position.sell(element.date, element.priceClose, 20);
    }
    std::cout << "Profit/Loss:  " << CPRX_Position.getProfitLoss() << std::endl;
    PORTFOLIO_H::Portfolio portfolio;
    portfolio.addPosition(CPRX_Position);
}
*/

int main() {
    // Stock class instance.
    STOCK_H::Stock stock_CPRX = Stock("CPRX");
    stock_CPRX.getHistoricalStockData("HistoricalData/HistoricalData_Year_CPRX.csv");
    stock_CPRX.calculateTechnicalIndicators();

    // Position class instance.
    POSITION_H::Position<Stock> CPRX_Position = Position<Stock>();

    // Portfolio class instance.
    PORTFOLIO_H::Portfolio portfolio;
    portfolio.addPosition(CPRX_Position);

    bool buySignalTrend = false;
    bool sellSignalTrend = false;

    unsigned short int _counter = 0;
    for (auto element : stock_CPRX.stockHistoricalDataDeque) {
        auto technicalIndicators = stock_CPRX.stockTechnicalIndicatorsMap[element.date];

        /// This is the Strategy implementation. Determine when to throw buy/sell signals using different technical indicators.

        // Determines if the start of the sequence is in a buy or sell signal.
        if (_counter == 0) {
            _counter += 1;
            if (technicalIndicators.movingAverageConvergenceDivergence > technicalIndicators.movingAverageConvergenceDivergenceSignal) {
                // Throw buy signal
                CPRX_Position.buy(element.date, element.priceClose, 100);
                std::cout << "BUY: " << element.date << "\n";
                buySignalTrend = true;
                continue;
            }
            else if (technicalIndicators.movingAverageConvergenceDivergence < technicalIndicators.movingAverageConvergenceDivergenceSignal) {
                // Throw sell signal
                CPRX_Position.sell(element.date, element.priceClose, 100);
                std::cout << "SELL: " << element.date << "\n";
                sellSignalTrend = true;
                continue;
            }
            else { std::cout << "MACD == MACD Signal.\n"; }
        }

        // Continue iteration until a buy signal is found if a current sell signal exists.
        if (sellSignalTrend) {
            // We're looking for a change in trend.
            if (technicalIndicators.movingAverageConvergenceDivergence > technicalIndicators.movingAverageConvergenceDivergenceSignal) {
                // Send buy signal
                std::cout << "BUY:  " << element.date << "\n";
                CPRX_Position.buy(element.date, element.priceClose, 100);
                buySignalTrend = true;
                sellSignalTrend = false;
                std::cout << "Profit/Loss:  " << CPRX_Position.getProfitLoss() << "\n\n";
            }
        }
        if (buySignalTrend) {
            if (technicalIndicators.movingAverageConvergenceDivergence < technicalIndicators.movingAverageConvergenceDivergenceSignal) {
                // Send sell signal
                std::cout << "SELL: " << element.date << "\n";
                CPRX_Position.sell(element.date, element.priceClose, 100);
                sellSignalTrend = true;
                buySignalTrend = false;
                std::cout << "Profit/Loss:  " << CPRX_Position.getProfitLoss() << "\n\n";
            }
        }
    }

    std::cout << "Program executed successfully\n>> ";
    std::cin.get();
    return 1;
}