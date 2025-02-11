#ifndef PORTFOLIO_H
#define PORTFOLIO_H

#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <cassert>
#include <variant>

#include "Stock.h"
#include "Option.h"
#include "Position.h"


class Portfolio {
    private:
        //std::vector<std::variant< POSITION_H::Position<Stock> >> positionHistory;
        // std::map<std::string, std::variant< POSITION_H::Position<Stock> >> positions;
        double totalAllocated = 0;
        // std::map<std::string, POSITION_H::Position<Stock> > positions;

        std::map<std::string, std::variant< Position<Stock>, Position<Option> >> positions;

    public:
        std::map<std::string, double> investmentAllocations;
        double portfolioVariance;

        //void addPosition(const std::variant< POSITION_H::Position<Stock> >& position);
        // void addPosition(std::string tickerSymbol, const std::variant< POSITION_H::Position<Stock> > position);
        
        // template <typename Instrument>
        // void addPosition(std::string tickerSymbol, const std::variant< POSITION_H::Position<Instrument> > position);

        template <typename Instrument>
        void addPosition(std::string tickerSymbol, const POSITION_H::Position<Instrument> position);

        // std::variant< POSITION_H::Position<Stock> > getPosition(std::string stockTicker);

        // POSITION_H::Position<Stock> getPosition(std::string stockTicker);
        std::variant<Position<Stock>, Position<Option>> getPosition(std::string stockTicker);

        void computePortfolioValue();
};

#endif