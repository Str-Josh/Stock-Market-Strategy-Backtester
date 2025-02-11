#include "Strategies.h"


Strategy::Strategy() {
    strategySignalFunctions["MACD"]["BUY"] = [](double MACD, double MACDS) { return (MACD > MACDS); };
    strategySignalFunctions["MACD"]["SELL"] = [](double MACD, double MACDS) { return (MACD < MACDS); };

    strategySignalFunctions["MovingAverageCrossover"]["BUY"] = [](double fastMA, double slowMA) { return (fastMA > slowMA); };
    strategySignalFunctions["MovingAverageCrossover"]["BUY"] = [](double fastMA, double slowMA) { return (fastMA < slowMA); };

    strategySignalFunctions["RSI"]["Overbought"] = [](double RSI_value) { return (RSI_value > 70); };
    strategySignalFunctions["RSI"]["Oversold"] = [](double RSI_value) { return (RSI_value < 30); };
}

Strategy::Strategy(std::string strategyName) {
    strategySignalFunctions["MACD"]["BUY"] = [](double MACD, double MACDS) { return (MACD > MACDS); };
    strategySignalFunctions["MACD"]["SELL"] = [](double MACD, double MACDS) { return (MACD < MACDS); };
    
    strategySignalFunctions["MovingAverageCrossover"]["BUY"] = [](double fastMA, double slowMA) { return (fastMA > slowMA); };
    strategySignalFunctions["MovingAverageCrossover"]["SELL"] = [](double fastMA, double slowMA) { return (fastMA < slowMA); };
    
    strategySignalFunctions["RSI"]["Overbought"] = [](double RSI_value) { return (RSI_value > 70); };
    strategySignalFunctions["RSI"]["Oversold"] = [](double RSI_value) { return (RSI_value < 30); };

    this->currentStrategyDetails = strategySignalFunctions[strategyName];
}

std::map<std::string, SignalFunctionUnion>& Strategy::operator[] (const std::string& strategyName) {
    if (this->strategySignalFunctions.count(strategyName) == 0) {
        throw std::out_of_range("The provided key was not found.");
    }
    return this->strategySignalFunctions[strategyName];
}

template<class... Fs>
struct Overload : public Fs... {
    using Fs::operator()...;
};

template<class... Fs>
Overload(Fs...) -> Overload<Fs...>;

bool Strategy::throwBuySignal(std::vector<double> functionArguments) {
    if (this->currentStrategyDetails.empty()) { throw std::runtime_error("Please setup a strategy."); }
    else {
        if (this->currentStrategyDetails.count("BUY") != 0) {
            SignalFunctionUnion possibleVariants = this->currentStrategyDetails["BUY"];

            return std::visit(
                Overload{
                    [&](std::function<bool(double)> functionCall) {
                        if (functionArguments.size() >= 1) {
                            return functionCall(functionArguments.at(0));
                        }
                        throw std::runtime_error("Not enough arguments for function with 1 parameter of type double.");
                    },
                    [&](std::function<bool(double, double)> functionCall) {
                        if (functionArguments.size() >= 2) {
                            return functionCall(functionArguments.at(0), functionArguments.at(1));
                        }
                        throw std::runtime_error("Not enough arguments for function with 2 parametes of type double.");
                    },
                    [&](std::function<bool(std::vector<double>)> functionCall) {
                        return functionCall(functionArguments);
                    }
                },
            possibleVariants);
        }
        else { throw std::runtime_error("Please ensure the selected strategy contains a 'BUY' option"); }
    }
}

bool Strategy::throwSellSignal(std::vector<double> functionArguments) {
    if (this->currentStrategyDetails.empty()) { throw std::runtime_error("Please setup a strategy."); }
    else {
        if (this->currentStrategyDetails.count("SELL") != 0) {
            SignalFunctionUnion possibleVariants = this->currentStrategyDetails["SELL"];

            return std::visit(
                Overload{
                    [&](std::function<bool(double)> functionCall) {
                        if (functionArguments.size() >= 1) {
                            return functionCall(functionArguments.at(0));
                        }
                        throw std::runtime_error("Not enough arguments for function with 1 parameter of type double.");
                    },
                    [&](std::function<bool(double, double)> functionCall) {
                        if (functionArguments.size() >= 2) {
                            return functionCall(functionArguments.at(0), functionArguments.at(1));
                        }
                        throw std::runtime_error("Not enough arguments for function with 2 parametes of type double.");
                    },
                    [&](std::function<bool(std::vector<double>)> functionCall) {
                        return functionCall(functionArguments);
                    }
                },
            possibleVariants);
            // */
        }
        else { throw std::runtime_error("Please ensure the selected strategy contains a 'SELL' option"); }
    }
}

void Strategy::addCustomConditions(std::string conditionName, std::variant<std::function<bool(double)>, std::function<bool(double, double)>, std::function<bool(std::vector<double>)>> condition) {
    if (this->currentStrategyDetails.empty()) {
        throw std::runtime_error("Please ensure a strategy is selected. Improvements upcoming.");
    }
    this->currentStrategyDetails[conditionName] = condition;
}

Backtester::Backtester(Strategy strategy) {
    this->selectedStrategy = strategy;
}

template <class T>
void Backtester::BacktestSimulator(Position<Stock> position, Strategy strategy, std::vector<std::function<T(const Stock::technicalIndicators&)>> indicatorsForConditions) {
    const Stock* stock_pointer = position.getAsset();
    Stock stock = *stock_pointer;

    unsigned short int signalTrend;

    unsigned short int sharesToBuySell = 100;

    unsigned short int sharesBought = 0;
    unsigned short int sharesSold = 0;

    unsigned short int _counter = 0;
    for (auto element : stock.stockHistoricalDataDeque) {
        auto technicalIndicators = stock.stockTechnicalIndicatorsMap[element.date];
        /// This is the Strategy implementation. Determine when to throw buy/sell signals using different technical indicators.
        
        std::vector<double> indicatorConditionValues;
        for (auto indicator : indicatorsForConditions) {
            indicatorConditionValues.push_back(indicator(technicalIndicators));
        }

        // Determines if the start of the sequence is in a buy or sell signal.
        if (_counter == 0) {
            if (strategy.throwBuySignal(indicatorConditionValues)) {
                position.buy(element.date, element.priceClose, sharesToBuySell);
                sharesBought += sharesToBuySell;
                signalTrend = 0;
                // std::cout << "BUY: " << element.date << "\n\n";
            }
            else if (strategy.throwSellSignal(indicatorConditionValues)) {
                int positionCurrentShares = position.getNumberShares();
                if (positionCurrentShares > 0) {
                    position.sell(element.date, element.priceClose, positionCurrentShares);
                    this->profitLossData.push_back(position.getProfitLoss());
                    // std::cout << "PL (after sell):  " << position.getProfitLoss() << "\n";
                    // std::cout << "SELL: " << element.date << "\n\n";
                }
                else { }  // Can't sell something you don't have.
                signalTrend = 1;
            }
            else {  }
            _counter += 1;
        }
        
        // Determine what to do with the particular trend... Either a buy trend (0) or sell trend (1).
        if (signalTrend == 1) {
            // It's currently a sell signal but we wanna see for the change in direction for a buy signal to be thrown.
            if (strategy.throwBuySignal(indicatorConditionValues)) {
                // std::cout << "Buy signal rating\n";
                position.buy(element.date, element.priceClose, sharesToBuySell);
                sharesBought += sharesToBuySell;
                signalTrend = 0;
                // std::cout << "BUY: " << element.date << "\n\n";
            }
        }
        else if (signalTrend == 0) {
            // It's currently a buy signal but we wanna see for the change in direction for a sell signal to be thrown.
            if (strategy.throwSellSignal(indicatorConditionValues)) {
                // std::cout << "Sell signal rating\n";
                int positionCurrentShares = position.getNumberShares();
                if (positionCurrentShares > 0) {
                    /// When we have enough shares so that we can sell.
                    position.sell(element.date, element.priceClose, sharesToBuySell);
                    this->profitLossData.push_back(position.getProfitLoss());  // since PL only changes when selling shares.
                    // std::cout << "PL (after sell):  " << position.getProfitLoss() << "\n";
                    // std::cout << "SELL: " << element.date << "\n\n";
                }
                else { /* in the event there aren't any shares, we have to wait to buy. */ }
                signalTrend = 1;
            }
        }
        _counter += 1;
    }
    this->backtestingCompleted = true;
}
template void Backtester::BacktestSimulator(Position<Stock> position, Strategy strategy, std::vector<std::function<double(const Stock::technicalIndicators&)>> indicatorsForConditions);

std::vector<double> Backtester::GetProfitLossFromBacktest() {
    if (!this->backtestingCompleted) {
        throw std::runtime_error("Backtesting has not been completed. Please wait for backtesting to complete prior to accessing profit/loss data.");
    }
    return this->profitLossData;
}