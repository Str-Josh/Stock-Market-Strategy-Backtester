//
// Author: Joshua Carter
//

#include <iostream>
#include <map>
#include <vector>

#include "Stock.h"
#include "Position.h"
#include "Portfolio.h"
#include "Strategies.h"

#include "../lib/json/single_include/nlohmann/json.hpp"

int main() {
    // Stock class instance.
    STOCK_H::Stock stock_CPRX = Stock("CPRX");
    STOCK_H::Stock stock_RIG  = Stock("RIG");
    stock_CPRX.getHistoricalStockData("HistoricalData/HistoricalData_Year_CPRX.csv");
    stock_CPRX.calculateTechnicalIndicators();
    stock_RIG.getHistoricalStockData("HistoricalData/HistoricalData_Year_RIG.csv");
    stock_RIG.calculateTechnicalIndicators();

    // Position class instance.
    POSITION_H::Position<Stock> CPRX_Position = Position<Stock>(&stock_CPRX);
    POSITION_H::Position<Stock> RIG_Position = Position<Stock>(&stock_RIG);

    // Portfolio class instance.
    PORTFOLIO_H::Portfolio portfolio;
    portfolio.addPosition("CPRX", CPRX_Position);
    portfolio.addPosition("RIG", RIG_Position);

    // Define our strategy to use.
    Strategy strategy_MACD = Strategy("MACD");
    Strategy strategy_RSI = Strategy("RSI");

    Backtester backtester = Backtester(strategy_MACD);
    // Backtester backtester = Backtester(strategy_RSI);

    auto macd = std::function<double(const Stock::technicalIndicators&)>([](const Stock::technicalIndicators& ti) { return ti.movingAverageConvergenceDivergence; });

    auto macds = std::function<double(const Stock::technicalIndicators&)>([](const Stock::technicalIndicators& ti) { return ti.movingAverageConvergenceDivergenceSignal; });

    auto rsi14 = std::function<double(const Stock::technicalIndicators&)>([](const Stock::technicalIndicators& ti) { return ti.relativeStrengthIndex; });
    
    auto sma20 = std::function<double(const Stock::technicalIndicators&)>([](const Stock::technicalIndicators& ti) { return ti.movingAverageDays20; });

    // std::vector<std::function<double(const Stock::technicalIndicators&)>> indicatorFunctionDefinitions = { rsi14 };

    // Define the functions to use in strategy using a vector:
    std::vector<std::function<double(const Stock::technicalIndicators&)>> indicatorFunctionDefinitions = { macd, macds };

    // Example demonstrating how to add custom conditions in the strategy.
    // strategy_RSI.addCustomConditions("BUY", [](double RSI_value) { return (RSI_value < 30); });
    // strategy_RSI.addCustomConditions("SELL", [](double RSI_value) { return (RSI_value > 70); });
    // strategy_RSI.addCustomConditions("BullishDivergence", [](std::vector<double> arguments__price_low_RSI_low) { return true; });

    // strategy_RSI.addCustomConditions("BUY", strategy_RSI["RSI"]);
    // strategy_RSI.addCustomConditions("SELL", strategy_RSI["RSI"]["SELL"]);

    // Backtest the strategy.
    // backtester.BacktestSimulator(CPRX_Position, strategy_RSI, indicatorFunctionDefinitions);
    backtester.BacktestSimulator(CPRX_Position, strategy_MACD, indicatorFunctionDefinitions);
    std::vector<double> profitLossData = backtester.GetProfitLossFromBacktest();

    /// Calculate Average Profit/Loss.
    double avgPL = 0;
    for (auto j = 0; j < profitLossData.size(); j++) { 
        avgPL += profitLossData.at(j);
        if (avgPL >= 5000 || avgPL <= -5000) { break; }  // to ensure we don't get scientific notation in results.
        // std::cout << "Average PL at " << j << ":  " << avgPL << "\n";
    }
    avgPL /= profitLossData.size();

    std::cout << "Below are the results of using your selected strategy for trades on historical data...\n\n";
    std::cout << "Profit/Loss after simulation:  " << profitLossData.at(profitLossData.size() - 1) << "\n";
    std::cout << "Average Profit/Loss:  " << avgPL << "\n";

    std::cout << "\nProgram executed successfully\n>> ";
    std::cin.get();
    return 1;
}