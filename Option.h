//
// Author: Joshua Carter
// Date: 01/23/2025
//

#ifndef OPTION_H
#define OPTION_H

#include "Stock.h"
#include <memory>

class Option {
    private:
        double strikePrice;
        std::string expirationDate;
        
        /// @brief Classify as "Put" or "Call" option.
        std::string optionType;
        
        /// @brief Classify as "American" or "European" option.
        std::string optionOrigin;
        
        /// @brief The underlying stock of the option.
        Stock* underlyingStock = nullptr;

    public:
        Option(const std::string stockTickerSymbol);
        Option(Stock stock);
        ~Option() { delete this->underlyingStock; }  // safe when called on null ptr.

        void setStrikePrice(double strikePrice);
        double getStrikePrice();

        void setExpirationDate(std::string expirationDate);
        std::string getExpirationDate();

        void setOptionType(std::string optionType);
        std::string getOptionType();
};

#endif