#include "apiRequestBuilder.h"

/// @brief Constructs the base url for the web API.
/// @param baseUrl the base url of the web API service.
apiRequestBuilder::apiRequestBuilder(const std::string baseUrl) {
    this->url = baseUrl;
}

/// @brief Accepts and parses query parameters. Doesn't parse beginning '?'.
/// @param queryParams a map of the key and value for the query parameter.
void apiRequestBuilder::establishQueryParams(std::map<std::string, std::string> queryParams) {
    std::lock_guard<std::mutex> lock(this->mtx);
    if (queryParams.empty()) {
        throw new std::invalid_argument("Please ensure there are query parameters.");
    }
    for (auto queryParam : queryParams) {
        /*
         * Works but for some reason puts the last param first.
         * No big deal now but might want to investigate later on.
        */
        if (this->queryParams.empty()) {
            this->queryParams = queryParam.first + "=" + queryParam.second + "&";
        }
        else {
            this->queryParams += queryParam.first + "=" + queryParam.second + "&";
        }
    }
    this->queryParams.pop_back();
    /*// For error checking...
    std::cout << "URL:  " << this->url << std::endl;
    std::cout << "Query:  " << this->queryParams << std::endl;
    std::cout << this->url + this->queryParams << std::endl;
    std::cout << std::endl;
    */
}

static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

/// @brief Sends an HTTP GET request to API.
/// @return Returns the JSON response object.
int apiRequestBuilder::send() {
    std::lock_guard<std::mutex> lock(this->mtx);
    curl_global_init(CURL_GLOBAL_ALL);
    CURL* curl = curl_easy_init();
    CURLcode res;

    // Create a string to store the response
    std::string response;
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, (this->url + this->queryParams).c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
        res = curl_easy_perform(curl);

        if (res != CURLE_OK) {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
        }
        else {
            try {
                nlohmann::json j = nlohmann::json::parse(response);
                // Now we can process the JSON response data.
                if (j.is_object()) {
                    for (auto& item : j.items()) {
                        // std::cout << item.key() << std::endl;  // Testing
                        this->jsonResponseKeys.push(item.key());
                    }
                    // std::cout << "Getting ready to set: \n";
                    std::unique_ptr<nlohmann::json> jsonDataObject = std::make_unique<nlohmann::json>(j);
                    // std::cout << "created new unique_ptr: \n";
                    this->jsonResponse = std::move(jsonDataObject);
                    // std::cout << "moved the json Data Object\n";
                }
                else {
                    std::cerr << "Error: JSON parsing failed" << "\n";
                }
            }
            catch (const nlohmann::json::exception& e) {
                std::cerr << "Failed to parse JSON: \n" << e.what() << std::endl;
            }
        }
        curl_easy_cleanup(curl);
    }
    curl_global_cleanup();
    return 1;
}

/// @brief Retrieves the API key from an arbitrary JSON file for encapsulation purposes.
/// @param webApiName The key within the JSON file which points to the API key.
/// @param jsonFileLocation The file location for the JSON file with the API key.
/// @return Returns the APIKEY.
std::string apiRequestBuilder::getApiKey(std::string webApiName, std::string jsonFileLocation) {
    std::lock_guard<std::mutex> lock(this->mtx);
    std::ifstream file(jsonFileLocation);
    nlohmann::json jsonObject;
    file >> jsonObject;
    // std::string apiKey = jsonObject[webApiName];
    return jsonObject[webApiName];
}

void apiRequestBuilder::saveResponseToFile() {
    std::lock_guard<std::mutex> lock(this->mtx);
    if (this->jsonResponse == NULL) {
        throw new std::logic_error("There is no response data available for access.");
    }
    // TODO: Save the response in a JSON file..
}

const nlohmann::json& apiRequestBuilder::getJsonResponseData() { 
    std::lock_guard<std::mutex> lock(this->mtx);
    return *this->jsonResponse;
}

void apiRequestBuilder::setJsonResponseData(nlohmann::json jsonData) {
    std::lock_guard<std::mutex> lock(this->mtx);
    std::cout << "Getting to set: \n";
    std::unique_ptr<nlohmann::json> jsonDataObject = std::make_unique<nlohmann::json>(jsonData);
    std::cout << "created new unique_ptr: \n";
    this->jsonResponse = std::move(jsonDataObject);
    std::cout << "moved the json Data Object\n";
}

/*  To test this ind class
int main() {
    //std::cout << "Starting:  \n";

    // #include <chrono>
    // auto start = std::chrono::high_resolution_clock::now();

    // auto end = std::chrono::high_resolution_clock::now();
    // auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    // std::cout << "Duration: " << duration.count() << " microseconds" << "\n";

    // Define our request url.
    std::string url = "https://www.alphavantage.co/query?";
    apiRequestBuilder request = apiRequestBuilder(url);
    // Build our query parameters.
    std::string apiKey = request.getApiKey("AlphaVantage");
    std::map<std::string, std::string> query {
        {"function", "SMA"},
        {"symbol", "IBM"},
        {"interval", "weekly"},
        {"time_period", "10"},
        {"series_type", "open"},
        {"apikey", apiKey},
    };
    // Now we build the request
    request.establishQueryParams(query);
    int success = request.send();
    std::cout << "\n\nProgram executed successfully";
    std::cin.get();  //  Keeps the console window open so I can see what's up.
    return 1;
}
*/