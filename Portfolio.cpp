#include "Portfolio.h"

Portfolio::Portfolio() {
    std::cout << "";
}

void Portfolio::addPosition(const std::variant< POSITION_H::Position<Stock> >& position) {
    positionHistory.push_back(position);
}

std::variant< POSITION_H::Position<Stock> > getPosition() {
    std::cout << "Get position";
    Position<Stock> stock;
    return stock;
}

void Portfolio::computePortfolioValue() {
    for(int positionIndex = 0; positionIndex <= positionHistory.size(); positionIndex++) {
        std::cout << "";
    }
    std::cout << "\nNot implemented: computePortfolioValue";
}

/*
int main() {
    POSITION_H::Position<Stock> rand;
    Portfolio port;
    port.addPosition(rand);
    return 1;
}
*/
