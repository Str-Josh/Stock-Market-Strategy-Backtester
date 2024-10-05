#ifndef API_REQUEST_BUILDER_H
#define API_REQUEST_BUILDER_H

#include <exception>
#include <fstream>
#include <iostream>
#include <string>
#include <queue>
#include <memory>
#include <mutex>

#include "../lib/json/single_include/nlohmann/json.hpp"

extern "C" {
    #include "../lib/curl_PreBuilt/include/curl/curl.h"
}

using namespace nlohmann;

class apiRequestBuilder {
    private:
        std::mutex mtx;
        std::string url;
        std::string queryParams;
        std::unique_ptr<nlohmann::json> jsonResponse = nullptr;
        std::queue<std::string> jsonResponseKeys;
    public:
        apiRequestBuilder(const std::string baseUrl);
        void establishQueryParams(std::map<std::string, std::string> queryParams);
        int send();
        std::string getApiKey(std::string webApiName, std::string jsonFileLocation = "keys.json");
        void saveResponseToFile();

        const nlohmann::json& getJsonResponseData();
        void setJsonResponseData(nlohmann::json jsonData);
};

#endif