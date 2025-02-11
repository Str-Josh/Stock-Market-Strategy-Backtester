#include "Portfolio.h"


///void Portfolio::addPosition(const std::variant< POSITION_H::Position<Stock> >& position) {
// void Portfolio::addPosition(std::string tickerSymbol, const std::variant< POSITION_H::Position<Stock> > position) {
// template <typename Instrument>
// void Portfolio::addPosition(std::string tickerSymbol, const std::variant< POSITION_H::Position<Instrument> > position) {
template <typename Instrument>
void Portfolio::addPosition(std::string tickerSymbol, const POSITION_H::Position<Instrument> position) {
    positions[tickerSymbol] = position;
    if (investmentAllocations.count(typeid(Instrument).name()) != 0) {
        investmentAllocations[typeid(Instrument).name()] += position.getNumberShares() * position.getBuyPrice();
    }
    else {
        investmentAllocations[typeid(Instrument).name()] = position.getNumberShares() * position.getBuyPrice();
    }
    this->totalAllocated += (position.getNumberShares() * position.getBuyPrice());
}
template void Portfolio::addPosition(std::string tickerSymbol, const Position<STOCK_H::Stock> position);
template void Portfolio::addPosition(std::string tickerSymbol, const Position<OPTION_H::Option> position);


// template void Portfolio::addPosition(std::string tickerSymbol, const Position<STOCK_H::Stock> position);
///template class Portfolio::addPosition<STOCK_H::Stock>(std::string tickerSymbol, const POSITION_H::Position<STOCK_H::Stock> position);

// std::variant< POSITION_H::Position<Stock> > Portfolio::getPosition(std::string stockTicker) {
std::variant<Position<Stock>, Position<Option>> Portfolio::getPosition(std::string stockTicker) {
    return positions[stockTicker];
}

void Portfolio::computePortfolioValue() {
    if (investmentAllocations.empty()) { throw std::runtime_error("Ensure Portfolio contains 1 or more positions."); }
    for (auto invAllocPair : investmentAllocations) {
        invAllocPair.second /= this->totalAllocated;  // yields the decimal form for weights for portfolio value.
    }
    // for (const auto& [tickerSymbol, position] : this->positions) {
    //     std::visit([](const auto& pos) {
    //         if constexpr (std::is_same_v(std::decay_t<decltype(pos)>, Position<Stock>>)) {
    //             //
    //         }
    //     }, position);
    //     if ()
    // }
}


/*
int main() {
    POSITION_H::Position<Stock> rand;
    Portfolio port;
    port.addPosition(rand);
    return 1;
}
*/