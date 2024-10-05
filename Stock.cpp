
#include "Stock.h"


nlohmann::json Stock::getApiData(std::string apiFunctionType, unsigned int timePeriod, std::map<std::string, std::string> customQueryParameters) {
    std::string url = "https://www.alphavantage.co/query?";
    apiRequestBuilder request = apiRequestBuilder(url);

    // Build our query parameters.
    std::map<std::string, std::string> query;

    if (customQueryParameters.empty()) {
        query = {
            {"function", apiFunctionType},
            {"symbol", this->tickerSymbol},
            {"interval", this->timeInterval},
            {"time_period", std::to_string(timePeriod)},
            {"series_type", "close"},
            {"apikey", request.getApiKey("AlphaVantage")},
        };
    }
    else { query = customQueryParameters; }

    request.establishQueryParams(query);
    int success = request.send();
    nlohmann::json jsonObjectData = request.getJsonResponseData();
    return jsonObjectData;
}

Stock::Stock(const std::string tickerSymbol) { this->tickerSymbol = tickerSymbol; }

std::string Stock::getTickerSymbol() { return this->tickerSymbol; }

void Stock::setStockExchange(std::string stockExchange) { this->stockExchange = stockExchange; }
std::string Stock::getStockExchange() { return this->stockExchange; }

void Stock::setStockSector(std::string stockSector) { this->sector = stockSector; }
std::string Stock::getStockSector() { return this->sector; }

void Stock::calculateFundamentalData() {
    /* Just the formulas for:
    * Price to earnings ratio
    * earnings per share
    */
   std::cout << "calculate fundamental data" << "\n";
}

void Stock::calculateTechnicalIndicators() {
    // Make sure we have a set of dates to use for data.
    if (this->stockHistoricalDataDeque.empty()) {
        throw new std::invalid_argument("The historical data deque is empty.\n");
    }

    std::string indicators[5] = {
        "SMA_20",
        "MACDEXT_20",
        "RSI_14",
        "ROC_20",
        "AROON_25"
    };

    nlohmann::json SMA20;
    nlohmann::json MACDEXT20;
    nlohmann::json RSI14;
    nlohmann::json ROC20;
    nlohmann::json AROON25;

    std::map<std::string, nlohmann::json> savedDataLoad = {
        { "SMA20", SMA20 },
        { "MACDEXT20", MACDEXT20 },
        { "RSI14", RSI14},
        { "ROC20", ROC20 },
        { "AROON25", AROON25 }
    };

    try {
        for (auto indicatorName : indicators) {
            std::string savedDataLoadKeyName = indicatorName;
            savedDataLoadKeyName.erase(indicatorName.find("_", 0), 1);

            std::ifstream savedDataFile("TechnicalIndicatorsData/Indicators-" + this->tickerSymbol + "-" + savedDataLoadKeyName + ".json");

            if (savedDataFile.good()) {
                // Data is good. Load it.
                if (savedDataFile.is_open()) {
                    json jsonObject;
                    savedDataFile >> jsonObject;
                    savedDataFile.close();

                    if (jsonObject.is_null()) {
                        std::cerr << "The JSON data file for: '" + savedDataLoadKeyName + "' was null.\n";
                    }
                    else if (jsonObject.contains("Information")) {
                        std::cerr << "API rate limit was exceeded. Please try again later.\n";
                        continue;
                    }
                    else if (jsonObject == NULL) {
                        std::cerr << "JSON data was NULL.\n";
                    }
                    else if (jsonObject["Meta Data"]["4: Interval"] != this->timeInterval) {
                        std::cerr << "The data in the '" + savedDataLoadKeyName + "' file has a mismatch with the expected time interval.\n";
                        continue;  // Let the method error out.
                    }
                    else {
                        savedDataLoad[savedDataLoadKeyName] = jsonObject;
                        continue;
                    }
                }
            }
            else {
                // Data is not good. Call API for some.
                std::cerr << "Couldn't find data file for: '" << savedDataLoadKeyName << "'. Calling API for data.\n";
            }
            // Get the technical indicator data from API call.

            // Get the technical indicator name we want for the API request.
            std::string functionType = indicatorName;
            functionType.erase(functionType.find("_"), functionType.size());
            unsigned int timePeriodValue;

            // Get time period desired for the API request.
            std::string timePeriod = indicatorName;
            timePeriod.erase(0, timePeriod.find("_") + 1);
            std::stringstream timePeriodStream(timePeriod);
            timePeriodStream >> timePeriodValue;

            // A test JSON object... Will use the API call function for prod.
            // std::ifstream testApiCall("TechnicalIndicatorsData/testsData/Indicators-" + this->tickerSymbol + "-" + savedDataLoadKeyName + ".json");
            json testJsonObject;
            // testApiCall >> testJsonObject;

            // Set our local variable to contain the data from the API call.
            savedDataLoad[savedDataLoadKeyName] = testJsonObject;  // this->getApiData(functionType, timePeriodValue);

            try {
                // Save the API data to a JSON file.
                std::ofstream dataFile("TechnicalIndicatorsData/Indicators-" + this->tickerSymbol + "-" + savedDataLoadKeyName + ".json");
                dataFile << savedDataLoad[savedDataLoadKeyName];
            }
            catch (std::exception& e) {
                std::cerr << "Error while saving data to JSON file:  " << e.what() << "\n";
            }
        }
    }
    catch (std::exception& e) {
        std::cerr << "Error while accessing data from JSON file:  " << e.what() << "\n";
    }

    try {
        // Some variables so that we can calculate other indicators with the same loop.
        // variables for calculating Directional Movement Index.
        double totalPlusDM = 0;
        double totalMinusDM = 0;
        double trueHigh = 0;
        double trueLow = 0;
        double TR = 0;
        double previousTR = 0;
        double totalTR = 0;

        /// for calculating Intraday Momentum Index:
        double upsum = 0;
        double downsum = 0;

        /// for calculating Negative Volume Index:
        double negativeVolumeIndex = 0;
        for (int historicalDataIndex = 0; historicalDataIndex < this->stockHistoricalDataDeque.size(); ++historicalDataIndex) {
            Stock::historicalDataStruct currentElement = this->stockHistoricalDataDeque.at(historicalDataIndex);
            Stock::historicalDataStruct previousElement;

            if (historicalDataIndex != 0) {
                previousElement = this->stockHistoricalDataDeque.at(historicalDataIndex - 1);
                int currentVolume = currentElement.volume;
                int previousVolume = previousElement.volume;
                double currentClosePrice = currentElement.priceClose;
                double previousClosePrice = previousElement.priceClose;

                /// Calculate On Balance Volume
                if (currentClosePrice > previousClosePrice) {
                    this->stockTechnicalIndicatorsMap[currentElement.date].onBalanceVolume = previousVolume + currentVolume;
                }
                else if (currentClosePrice < previousClosePrice) {
                    this->stockTechnicalIndicatorsMap[currentElement.date].onBalanceVolume = previousVolume - currentVolume;
                }
                else if (currentClosePrice == previousClosePrice) {
                    this->stockTechnicalIndicatorsMap[currentElement.date].onBalanceVolume = previousVolume;
                }
                else {
                    std::cerr << "Error occurred when calculating On-Balance Volume Technical Indicator\n";
                }

                /// Calculate Chande Momentum Oscillator
                double chandeMomentumOscillatorUp = 0;
                double chandeMomentumOscillatorDown = 0;
                double ups_i = 0;
                double downs_i = 0;
                if (currentClosePrice > previousClosePrice) {
                    chandeMomentumOscillatorUp = currentClosePrice - previousClosePrice;
                }
                else if (currentClosePrice < previousClosePrice) {
                    chandeMomentumOscillatorDown = previousClosePrice - currentClosePrice;
                }
                else if (currentClosePrice == previousClosePrice) { }  // To ensure error is not thrown.
                else {
                    std::cerr << "Error occurred when calculating Chande Momentum Oscillator Technical Indicator.\n";
                }
                /*
                for (int indexCounter = historicalDataIndex - this->stockHistoricalDataDeque.size(); indexCounter <= this->stockHistoricalDataDeque.size(); indexCounter++) {
                    ups_i += chandeMomentumOscillatorUp;
                    downs_i += chandeMomentumOscillatorDown;
                }
                double CMO = 100 * ( (ups_i - downs_i) / (ups_i + downs_i) );
                */
                
                /// Calculate Directional Movement Index (+DI & -DI)
                double plusDM = 0;
                double minusDM = 0;
                double deltaHigh = previousElement.high - currentElement.high;
                double deltaLow = currentElement.low - previousElement.low;
                if ( (deltaHigh < 0 && deltaLow < 0) || (deltaHigh == deltaLow) ) {
                    plusDM = 0;
                }
                else if (deltaHigh > deltaLow) {
                    plusDM = deltaHigh;
                }
                else if (deltaHigh < deltaLow) {
                    minusDM = deltaLow;
                }
                else {
                    std::cerr << "Error occurred when calculating Directional Movement Index Technical Indicator.\n";
                }
                totalPlusDM = totalPlusDM - ( totalPlusDM / this->stockHistoricalDataDeque.size() ) + plusDM;
                totalMinusDM = totalMinusDM - ( totalMinusDM / this->stockHistoricalDataDeque.size() ) + minusDM;

                trueHigh = 0;
                // Get the highest value of the current element high or the previous close price.
                if (currentElement.high > previousClosePrice) { trueHigh = currentElement.high; }
                else { trueHigh = previousClosePrice; }

                trueLow = 0;
                // Get the lowest value from current element's low or the previous close price.
                if (currentElement.low > previousClosePrice) { trueLow = currentElement.low; }
                else { trueLow = previousClosePrice; }

                TR = trueHigh - trueLow;
                totalTR += TR - ( TR / this->stockHistoricalDataDeque.size() ) + TR;
                previousTR = TR;

                /// Calculate Intraday Momentum Index
                if (currentElement.priceClose > currentElement.priceOpen) {
                    upsum = upsum + (currentElement.priceClose - currentElement.priceOpen);
                }
                else {
                    downsum = downsum + (currentElement.priceOpen - currentElement.priceClose);
                }
                this->stockTechnicalIndicatorsMap[currentElement.date].intradayMomentumIndex = 100 * ( (upsum) / ( upsum + downsum ) );

                /// Calculate Ease of Movement
                double midpointMove = ( ( currentElement.high - currentElement.low ) / 2 ) - ( ( previousElement.high - previousElement.low ) / 2 );
                double boxRatio = ( (currentElement.volume / 10000) / (currentElement.high - currentElement.low) );
                this->stockTechnicalIndicatorsMap[currentElement.date].easeOfMovement = ( midpointMove ) / ( boxRatio );

                /// Calculate Momentum
                this->stockTechnicalIndicatorsMap[currentElement.date].momentum = currentElement.priceClose - previousElement.priceClose;

                /// Calculate Money Flow Index
                if (currentElement.typicalPrice > previousElement.typicalPrice) {
                    /* Do this later. I'm tired. */
                }

                /// Calculate Negative Volume Index
                if (currentElement.volume < previousElement.volume) {
                    negativeVolumeIndex = negativeVolumeIndex + ( (currentClosePrice - previousClosePrice) / previousClosePrice );
                }
                else {
                    negativeVolumeIndex = negativeVolumeIndex;
                }
            }

            this->stockTechnicalIndicatorsMap[currentElement.date].movingAverageDays20 = std::stod(std::string(savedDataLoad["SMA20"]["Technical Analysis: SMA"][currentElement.date]["SMA"]));
            this->stockTechnicalIndicatorsMap[currentElement.date].movingAverageConvergenceDivergence = std::stod(std::string(savedDataLoad["MACDEXT20"]["Technical Analysis: MACDEXT"][currentElement.date]["MACD"]));
            this->stockTechnicalIndicatorsMap[currentElement.date].movingAverageConvergenceDivergenceSignal = std::stod(std::string(savedDataLoad["MACDEXT20"]["Technical Analysis: MACDEXT"][currentElement.date]["MACD_Signal"]));
            this->stockTechnicalIndicatorsMap[currentElement.date].relativeStrengthIndex = std::stod(std::string(savedDataLoad["RSI14"]["Technical Analysis: RSI"][currentElement.date]["RSI"]));
            this->stockTechnicalIndicatorsMap[currentElement.date].rateOfChange = std::stod(std::string(savedDataLoad["ROC20"]["Technical Analysis: ROC"][currentElement.date]["ROC"]));
            this->stockTechnicalIndicatorsMap[currentElement.date].aroonUp = std::stod(std::string(savedDataLoad["AROON25"]["Technical Analysis: AROON"][currentElement.date]["Aroon Up"]));
            this->stockTechnicalIndicatorsMap[currentElement.date].aroonDown = std::stod(std::string(savedDataLoad["AROON25"]["Technical Analysis: AROON"][currentElement.date]["Aroon Down"]));
            this->stockTechnicalIndicatorsMap[currentElement.date].aroonOscillator = this->stockTechnicalIndicatorsMap[currentElement.date].aroonUp - this->stockTechnicalIndicatorsMap[currentElement.date].aroonDown;

            /// Actually calculate the other indicators.
            double plusDI = 100 * ( totalPlusDM / totalTR );
            double minusDI = 100 * ( totalMinusDM / totalTR );

            this->stockTechnicalIndicatorsMap[currentElement.date].plusDI = 100 * ( totalPlusDM / totalTR );
            this->stockTechnicalIndicatorsMap[currentElement.date].minusDI = 100 * ( totalMinusDM / totalTR );
            this->stockTechnicalIndicatorsMap[currentElement.date].DX = ( ( (plusDI) - (minusDI) ) / ( (plusDI) + (minusDI) ) );

            // /// Calculate Ease of Movement
            // double midpointMove = ( ( currentElement.high - currentElement.low ) / 2 ) - ( ( previousElement.high - previousElement.low ) / 2 );
            // double boxRatio = ( (currentElement.volume / 10000) / (currentElement.high - currentElement.low) );
            // this->stockTechnicalIndicatorsMap[currentElement.date].easeOfMovement = ( midpointMove ) / ( boxRatio );

            // Other calculations
            this->stockTechnicalIndicatorsMap[currentElement.date].marketFacilitationIndex = ( currentElement.high - currentElement.low ) / currentElement.volume;
            // this->stockTechnicalIndicatorsMap[currentElement.date].momentum = currentElement.priceClose - previousElement.priceClose;
        }
    }
    catch (std::exception& e) {
        std::cerr << "An error occurred while trying to set struct values:  " << e.what() << "\n";
    }
}

std::deque<Stock::historicalDataStruct> Stock::getHistoricalStockData(std::string filename) {
    std::deque<Stock::historicalDataStruct> dataVector;
    std::ifstream dataFile(filename);

    if (!dataFile.is_open()) {
        throw std::runtime_error("Could not open file");
    }
    std::string line, dateValue, columnName;
    std::vector<std::string> columnNamesVector;
    double val;

    if (dataFile.good()) {
        // Get first line in file
        std::getline(dataFile, line);
        std::stringstream stringStream(line);

        // Get each column name
        while (std::getline(stringStream, columnName, ',')) {
            columnNamesVector.push_back(columnName);
        }
    }

    while (std::getline(dataFile, line)) {
        std::stringstream currentLine(line);

        struct Stock::historicalDataStruct fileDataStructInstance;
        int columnIdx = 0;

        std::getline(currentLine, fileDataStructInstance.date, ',');

        while (currentLine >> val) {
            try {
                if (currentLine.fail()) {
                    std::cerr << "Error converting value to double at column" << columnIdx << "\n";
                }
                else {
                    if (columnNamesVector.at(columnIdx + 1) == "Open") {
                        fileDataStructInstance.priceOpen = val;
                    }
                    else if (columnNamesVector.at(columnIdx + 1) == "High") {
                        fileDataStructInstance.high = val;
                    }
                    else if (columnNamesVector.at(columnIdx + 1) == "Low") {
                        fileDataStructInstance.low = val;
                    }
                    else if (columnNamesVector.at(columnIdx + 1) == "Close") {
                        fileDataStructInstance.priceClose = val;
                    }
                    else if (columnNamesVector.at(columnIdx + 1) == "Adj Close") {
                        fileDataStructInstance.adjustedClosePrice = val;
                    }
                    else if (columnNamesVector.at(columnIdx + 1) == "Volume") {
                        fileDataStructInstance.volume = val;
                    }
                }

                if (currentLine.peek() == ',') {
                    currentLine.ignore();
                }
                columnIdx++;
            }
            catch (std::exception& e) {
                std::cerr << e.what() << std::endl;
            }
        }
        // Calculate the average price for the stock.
        fileDataStructInstance.averagePrice = (fileDataStructInstance.priceOpen + fileDataStructInstance.high + fileDataStructInstance.low + fileDataStructInstance.priceClose) / 4;

        // Calculate the median price for the stock.
        fileDataStructInstance.medianPrice = (fileDataStructInstance.high + fileDataStructInstance.low) / 2;

        // Calculate the typical price for the stock.
        fileDataStructInstance.typicalPrice = (fileDataStructInstance.high + fileDataStructInstance.low + fileDataStructInstance.priceClose) / 3;

        // Should I have money flow here or in technical indicators struct?
        fileDataStructInstance.moneyFlow = fileDataStructInstance.typicalPrice * fileDataStructInstance.volume;

        // Calculate Closing Location Value for the stock.
        double closingLocationValue = ((fileDataStructInstance.priceClose - fileDataStructInstance.low) - (fileDataStructInstance.high - fileDataStructInstance.priceClose)) / (fileDataStructInstance.high - fileDataStructInstance.low);

        // Calculate Chaikin Money Flow for the stock.
        // CMF_i = ( SUM_(i-n)^i (CLV * volume) ) / ( SUM_(i-n)^i (volume) )

        // Calculate percent change for the stock price.
        if (dataVector.size() > 0) {
            struct Stock::historicalDataStruct previousDatum = dataVector.at(dataVector.size() - 1);
            double percentChange = ( ( fileDataStructInstance.priceClose - previousDatum.priceClose ) / previousDatum.priceClose ) * 100;
            fileDataStructInstance.percentChange = percentChange;
        }
        else { }
        dataVector.push_back(fileDataStructInstance);
    }
    dataFile.close();
    this->stockHistoricalDataDeque = dataVector;
    return dataVector;
}

/*
int main() {
    Stock stock = Stock("CPRX");
    stock.getHistoricalStockData("HistoricalData/HistoricalData_Year_CPRX.csv");
    stock.calculateTechnicalIndicators();
    std::cout << "Completed.\n>> ";
    std::cin.get();
    return 1;
}
*/
