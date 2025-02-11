//
// Author: Joshua Carter
// Date: 08/16/2024
//

#include "Position.h"


template <class AssetType>
POSITION_H::Position<AssetType>::Position() {
    this->PL = 0;
}

template <class AssetType>
POSITION_H::Position<AssetType>::Position(const AssetType* asset) {
    this->setAsset(asset);
    this->PL = 0;
}

template <class AssetType>
const AssetType* POSITION_H::Position<AssetType>::getAsset() const { return this->asset; }

template <class AssetType>
void POSITION_H::Position<AssetType>::setAsset(const AssetType* asset) { this->asset = asset; this->assetTypename = typeid(AssetType).name(); }

template <class AssetType>
int POSITION_H::Position<AssetType>::getNumberShares() const { return this->numberShares; }

template <class AssetType>
void POSITION_H::Position<AssetType>::setAccountBalance(const double accountBalance) { this->accountBalance = accountBalance; }

template <class AssetType>
double POSITION_H::Position<AssetType>::getAccountBalance() const { return this->accountBalance; }

template <class AssetType>
double POSITION_H::Position<AssetType>::getBuyPrice() const { return this->buyPrice; }

template <class AssetType>
void POSITION_H::Position<AssetType>::buy(std::string buyDate, double sharePrice, unsigned int sharesToBuy) {
    if (this->entranceDate.empty()) {
        // if this is the first open of a trade.
        this->entranceDate = buyDate;
    }
    this->buyPrice = sharePrice;
    this->buyDate = buyDate;
    this->positionPrice += this->positionPrice + (sharePrice * sharesToBuy);
    this->averageSharePrice = this->positionPrice / (this->numberShares + sharesToBuy);
    this->numberShares += sharesToBuy;
}

template <class AssetType>
void POSITION_H::Position<AssetType>::sell(std::string sellDate, double sellPrice, unsigned int sharesToSell) {
    if (sharesToSell > this->numberShares) {
        std::cerr << "Attempted to sell more shares than were found available. {" << this->numberShares << "}\n";
    }
    double revenue = sellPrice * sharesToSell;
    this->PL += revenue - (sharesToSell * this->averageSharePrice);
    this->sellDate = sellDate;
    this->sellPrice = sellPrice;
    this->positionPrice -= this->averageSharePrice * sharesToSell;
    this->numberShares -= sharesToSell;
}

template <class AssetType>
double POSITION_H::Position<AssetType>::getProfitLoss() {
    return this->PL;
}

template <class AssetType>
double POSITION_H::Position<AssetType>::getAverageSharePrice() const {
    return this->averageSharePrice;
}

template <class AssetType>
std::string Position<AssetType>::getEntranceDate() const {
    return this->entranceDate;
}

template class Position<STOCK_H::Stock>;
template class Position<OPTION_H::Option>;