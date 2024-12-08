# Stock Market Strategy Backtester
Backtesting system for various financial instruments (e.g., stocks, options, bonds, ETFs, etc.)

## Developed Using
* __C++__
* __JSON__
* __nlohmann/json__
* __libcurl__
* AlphaVantage API

## Getting Started
### Prerequisites
* C++ Compiler (Clang, G++, etc.) I personally used G++

### Installation
1. Get a free API key at https://www.alphavantage.co/support/#api-key
2. Clone the repo: <br> `git clone https://github.com/Str-Josh/Financial-Derivatives-Strategy-Backtester.git`
3. Change git remote url to avoid accidental pushes to main project
<br>`git remote set-url origin github_username/repo_name`
<br>`git remote -v`
4. Install nlohmann/JSON and CURL
5. Create a file called "keys.json" in the local project directory
6. Enter the following in the keys.json file:
`{
    "AlphaVantage": "YOUR-API-KEY-GOES-HERE"
}`
11. Compile the following scripts:<br>
`g++ main.cpp -c`
`g++ Stock.cpp -c`
`g++ Position.cpp -c`
`g++ Portfolio.cpp -c`
`g++ apiRequestBuilder.cpp -c`
13. Create the executable script using the following (Assuming you're using G++):<br>
`g++ main.o Stock.o Position.o Portfolio.o apiRequestBuilder.o -L"PATH-TO-LIBCURL-LIBRARY\lib" -lcurl -o BacktestingSystem`
14. Go to project local directory and execute `BacktestingSystem.exe`


Lessons Learned:
* Implementing various C++ data structures (deque, map, vector, ...)
* Installation and integration of third-party libraries
* How to avoid threads from interferring and ensure thread safety
* Developing and using API requests
* Building and applying C++ templates
* Manipulation of JSON objects
* Usage of Smart Pointers
* File handling in C++
* Proper usage of include guards
* Compilers and necessary options in terminal


Future Additions/Improvements:
* Generalize the main.cpp to allow for other strategy implementations determined by user selection
    * Plan to do this by creating a Strategy class that has methods for checking if should throw a buy signal or sell signal where these methods would be called in the main.cpp if conditions
* Implement other calculations useful for determining a stock's long term performance (such as variance, ROI, and Sharpe Ratio)
* Implement classes for other financial instruments such as Mutual Funds or ETFs
* Create visualizations of Stock trends or profit loss changes
* Create a GUI using Qt so that there can be a dashboard for non-technical users
* Implement Company data analysis using API requests
