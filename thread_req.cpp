#include <iostream>
#include <curl/curl.h>
#include <algorithm>
#include <vector>
#include <thread>
#include <map>

// Callback function to handle the received data
size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output) {
    size_t total_size = size * nmemb;
    output->append((char*)contents, total_size);
    return total_size;
}

// Function to perform a GET request to the specified API URL
void Request(std::string & response, std::string apiUrl) {
    CURL* curl;
    CURLcode res;

    // Initialize cURL
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if (curl) {
        // Set the URL
        curl_easy_setopt(curl, CURLOPT_URL, apiUrl.c_str());

        // Set the callback function to handle the response data
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        // Perform the GET request
        res = curl_easy_perform(curl);

        // Check for errors
        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        }

        // Clean up
        curl_easy_cleanup(curl);
    }

    // Cleanup cURL global state
    curl_global_cleanup();

}

std::map<std::string, std::string> ThreadRequest(std::vector<std::string> Keys, std::vector<std::string> URLS)
{
    std::vector<std::thread> threads;
    std::map<std::string, std::string> data;

    for(int i = 0; i < URLS.size(); ++i){
        data[Keys[i]] = "";
        threads.emplace_back(Request, std::ref(data[Keys[i]]), URLS[i]);
    }

    for(auto & threading : threads){
        threading.join();
    }
    return data;
}

int main() {
    
    std::vector<std::string> URLS = {
        "https://www.bitmex.com/api/v1/orderBook/L2?symbol=XBT&depth=100",
        "https://www.bitmex.com/api/v1/orderBook/L2?symbol=XRP&depth=100",
        "https://www.bitmex.com/api/v1/orderBook/L2?symbol=ETH&depth=100",
        "https://www.bitmex.com/api/v1/orderBook/L2?symbol=ADA&depth=100",
        "https://www.bitmex.com/api/v1/orderBook/L2?symbol=SOL&depth=100",
        "https://www.bitmex.com/api/v1/orderBook/L2?symbol=LTC&depth=100",
        "https://www.bitmex.com/api/v1/orderBook/L2?symbol=LINK&depth=100",
        "https://www.bitmex.com/api/v1/orderBook/L2?symbol=DOGE&depth=100"
    };

    std::vector<std::string> coins = {
        "XBT",
        "XRP",
        "ETH",
        "ADA",
        "SOL",
        "LTC",
        "LINK",
        "DOGE"
    };
    
    std::map<std::string, std::string> data = ThreadRequest(coins, URLS);

    for(auto & line : data){
        std::cout << line.first << "\t" << line.second << std::endl;
    }

    return 0;
}


