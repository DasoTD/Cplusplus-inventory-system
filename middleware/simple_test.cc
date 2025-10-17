#include <iostream>
#include <drogon/HttpClient.h>
#include <json/json.h>
#include <chrono>
#include <thread>

using namespace drogon;

int main() {
    std::cout << "Testing server connection..." << std::endl;
    
    // Create JSON payload
    Json::Value product;
    product["name"] = "Test Product";
    product["description"] = "A test product description";
    product["price"] = 29.99;
    product["quantity"] = 100;
    
    Json::StreamWriterBuilder builder;
    std::string jsonString = Json::writeString(builder, product);
    
    std::cout << "JSON payload: " << jsonString << std::endl;
    
    // Create HTTP client and request
    auto client = HttpClient::newHttpClient("http://127.0.0.1:7777");
    auto req = HttpRequest::newHttpRequest();
    req->setMethod(Post);
    req->setPath("/api/products");
    req->setBody(jsonString);
    req->setContentTypeCode(CT_APPLICATION_JSON);
    
    std::cout << "Sending request..." << std::endl;
    
    bool requestCompleted = false;
    
    client->sendRequest(req, [&](ReqResult result, const HttpResponsePtr &response) {
        std::cout << "Response received!" << std::endl;
        std::cout << "Result: " << (int)result << std::endl;
        
        if (result == ReqResult::Ok) {
            std::cout << "Status Code: " << response->getStatusCode() << std::endl;
            std::cout << "Response Body: " << response->getBody() << std::endl;
        } else {
            std::cout << "Request failed with result: " << (int)result << std::endl;
        }
        
        requestCompleted = true;
    });
    
    // Wait for response (with timeout)
    int waitTime = 0;
    while (!requestCompleted && waitTime < 15) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        waitTime++;
        std::cout << "Waiting... " << waitTime << "s" << std::endl;
    }
    
    if (!requestCompleted) {
        std::cout << "Request timed out!" << std::endl;
        return 1;
    }
    
    std::cout << "Test completed!" << std::endl;
    return 0;
}