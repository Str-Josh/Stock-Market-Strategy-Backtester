#ifndef STRATEGIES_H
#define STRATEGIES_H

#include <iostream>
#include <map>
#include <functional>
#include <variant>

#include "Position.h"
#include "Stock.h"

using SignalFunctionUnion = std::variant<std::function<bool(double)>, std::function<bool(double, double)>, std::function<bool(std::vector<double>)>>;

class Strategy {
    private:    
        std::map<std::string, std::map<std::string, SignalFunctionUnion>> strategySignalFunctions;
        std::map<std::string, SignalFunctionUnion> currentStrategyDetails = {};
    
    public:
        // Constructors.
        Strategy();
        Strategy(std::string strategyName);

        // Access specific strategy. (Could be useful for expanding to different analyze techniques)
        std::map<std::string, SignalFunctionUnion>& operator[] (const std::string& strategyName);

        // Throw signal based on set strategy.
        bool throwBuySignal(std::vector<double> functionArguments);
        bool throwSellSignal(std::vector<double> functionArguments);

        // Change a particular aspect of a selected strategy.
        void modifyBuyCondition();
        void modifySellCondition();
        void modifyNeutralCondition();

        void modifyOpenPositionCondition();
        void modifyClosePositionCondition();

        // Create your own custom condition.
        void addCustomConditions(std::string conditionName, std::variant<std::function<bool(double)>, std::function<bool(double, double)>, std::function<bool(std::vector<double>)>> condition);

        void setStrategyDetails();
};


class Backtester {
    private:
        bool backtestingCompleted = false;
        std::vector<double> profitLossData;
        Strategy selectedStrategy;

    public:
        Backtester(Strategy customStrategy);

        template <class T>
        void BacktestSimulator(Position<Stock> position, Strategy strategy, std::vector<std::function<T(const Stock::technicalIndicators&)>> indicatorsForConditions);

        std::vector<double> GetProfitLossFromBacktest();
};

#endif