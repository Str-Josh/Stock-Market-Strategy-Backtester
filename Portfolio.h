#ifndef PORTFOLIO_H
#define PORTFOLIO_H

#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <cassert>
#include <variant>

#include "Stock.h"
#include "Position.h"


class Portfolio {
    private:
        std::vector<std::variant< POSITION_H::Position<Stock> >> positionHistory;

    public:
        Portfolio();

        void addPosition(const std::variant< POSITION_H::Position<Stock> >& position);

        void computePortfolioValue();
};

#endif