# Financial Derivatives Strategy Backtester
Backtesting system for various financial derivatives (e.g., stocks, options, bonds, ETFs, etc.)

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
2. Clone the repo: `git clone https://github.com/Str-Josh/Financial-Derivatives-Strategy-Backtester.git`
3. Install nlohmann/JSON and CURL
4. Create a file called "keys.json" in the local project directory
5. Enter the following in the keys.json file:
`{
    "AlphaVantage": "YOUR-API-KEY-GOES-HERE"
}`
7. Change git remote url to avoid accidental pushes to main project
`git remote set-url origin github_username/repo_name`
`git remote -v`
9. 


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
