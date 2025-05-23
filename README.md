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
* C++ Compiler (Clang, G++, etc.) I personally used G++.

### Installation
1. Get a free API key at https://www.alphavantage.co/support/#api-key
2. Clone the repo: <br> `git clone https://github.com/Str-Josh/Financial-Derivatives-Strategy-Backtester.git`
3. Change git remote url to avoid accidental pushes to main project
<br>`git remote set-url origin github_username/repo_name`
<br>`git remote -v`
4. Install [nlohmann/json](https://github.com/nlohmann/json) and [CURL](https://curl.se/windows/)
5. Create a file called "keys.json" in the local project directory
6. Enter the following in the keys.json file:
`{
    "AlphaVantage": "YOUR-API-KEY-GOES-HERE"
}`
11. Compile the following scripts:<br>
`g++ -c main.cpp`
`g++ -c Stock.cpp`
`g++ -c Strategies.cpp`
`g++ -c Position.cpp`
`g++ -c Portfolio.cpp`
`g++ -c Option.cpp`
`g++ -c apiRequestBuilder.cpp`
13. If you're using Windows, link the object files using the following (Assuming you're using G++):<br>
`g++ main.o Stock.o Strategies.o Position.o Portfolio.o Option.o apiRequestBuilder.o -L"PATH-TO-LIBCURL-LIBRARY\lib" -lcurl -o BacktestingSystem`
13. If you're using Linux, ensure that you have linux-native libcurl version installed using:<br>
`sudo apt install libcurl4-openssl-dev libnghttp2-dev`
    Then link the object files using the following:<br>
    `g++ main.o Stock.o Strategies.o Position.o Portfolio.o Option.o apiRequestBuilder.o -lcurl -lnghttp2 -o BacktestingSystem`
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
* Create a GUI using Qt so that there can be a dashboard for non-technical users
* Implement other calculations useful for determining a stock's long term performance (such as variance, ROI, and Sharpe Ratio)
* Implement classes for other financial instruments such as Mutual Funds or ETFs
* Create visualizations of Stock trends or profit loss changes
* Implement Company data analysis using API requests
