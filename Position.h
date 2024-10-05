#ifndef POSITION_H
#define POSITION_H

#include <iostream>
#include "Stock.h"

/// @brief A holding of a specific asset within a portfolio.
/// @tparam AssetType Stock, Option, Mutual Fund, etc.
template <class AssetType>
class Position {
    private:
        const AssetType* asset;

        double accountBalance = 0;
        double positionPrice = 0;  // represents the amount of money from the account balance held within this position.
        double averageSharePrice = 0;  // average cost of all shares { 2 @ $5 , 3 @ $4 = ( (2 * 5) + (3 * 4) ) / (2 + 3) }

        unsigned int numberShares = 0;
        double PL;

        std::string entranceDate;

        std::string buyDate;
        double buyPrice;

        std::string sellDate;
        double sellPrice;

    public:
        Position();
        Position(const AssetType* asset);
        const AssetType* getAsset() const;
        void setAsset(const AssetType* asset);
        int getNumberShares() const;
        void setAccountBalance(const double accountBalance);
        double getAccountBalance() const;

        // void enterPosition(std::string buyDate, double sharePrice, unsigned int numberShares);
        // void exitPosition(std::string exitDate, double exitPrice);
        void buy(std::string buyDate, double sharePrice, unsigned int sharesToBuy);
        void sell(std::string sellDate, double sellPrice, unsigned int sharesToSell);

        double getProfitLoss();
        double getAverageSharePrice() const;
        std::string getEntranceDate() const;
};

#endif