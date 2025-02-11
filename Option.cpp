#include "Option.h"


Option::Option(const std::string stockTickerSymbol) {
    this->underlyingStock = new Stock(stockTickerSymbol);
}

Option::Option(Stock stock) {
    this->underlyingStock = &stock;
}

void Option::setStrikePrice(double strikePrice) { this->strikePrice = strikePrice; }
double Option::getStrikePrice() { return this->strikePrice; }

void Option::setExpirationDate(std::string expirationDate) { this->expirationDate = expirationDate; }
std::string Option::getExpirationDate() { return this->expirationDate; }

void Option::setOptionType(std::string optionType) { (optionType == "Put" || optionType == "Call") ? this->optionType = optionType : throw std::runtime_error("Must choose option type as 'Put' or 'Call'."); }
std::string Option::getOptionType() { return this->optionType; }
