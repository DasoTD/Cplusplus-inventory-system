#include <drogon/drogon.h>
#include <drogon/HttpClient.h>
#include <json/json.h>
#include <gtest/gtest.h>
#include <future>
#include <chrono>

using namespace drogon;

class ValidationMiddlewareTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Test server should be running with middleware enabled
    }
};

TEST_F(ValidationMiddlewareTest, ValidProductCreation) {
    Json::Value product;
    product["sku"] = "TEST001";
    product["name"] = "Test Product";
    product["description"] = "A test product description";
    product["unit_price"] = 29.99;
    product["quantity_in_stock"] = 100;
    product["reorder_threshold"] = 10;
    
    Json::StreamWriterBuilder builder;
    std::string jsonString = Json::writeString(builder, product);
    
    auto client = HttpClient::newHttpClient("http://127.0.0.1:7777");
    auto req = HttpRequest::newHttpRequest();
    req->setMethod(Post);
    req->setPath("/api/products");
    req->setBody(jsonString);
    req->setContentTypeCode(CT_APPLICATION_JSON);
    
    std::promise<void> promise;
    auto future = promise.get_future();
    
    client->sendRequest(req, [&](ReqResult result, const HttpResponsePtr &response) {
        try {
            ASSERT_EQ(result, ReqResult::Ok);
            ASSERT_EQ(response->getStatusCode(), k201Created);
            
            auto json = response->getJsonObject();
            ASSERT_TRUE(json);
            ASSERT_FALSE((*json)["error"].asBool());
            
            promise.set_value();
        } catch (...) {
            promise.set_exception(std::current_exception());
        }
    });
    
    auto status = future.wait_for(std::chrono::seconds(10));
    ASSERT_EQ(status, std::future_status::ready);
    future.get(); // This will rethrow any exceptions
}

TEST_F(ValidationMiddlewareTest, InvalidProductCreationMissingName) {
    Json::Value product;
    product["sku"] = "TEST002";
    product["description"] = "A test product description";
    product["unit_price"] = 29.99;
    product["quantity_in_stock"] = 100;
    product["reorder_threshold"] = 10;
    // Missing "name" field
    
    Json::StreamWriterBuilder builder;
    std::string jsonString = Json::writeString(builder, product);
    
    auto client = HttpClient::newHttpClient("http://127.0.0.1:7777");
    auto req = HttpRequest::newHttpRequest();
    req->setMethod(Post);
    req->setPath("/api/products");
    req->setBody(jsonString);
    req->setContentTypeCode(CT_APPLICATION_JSON);
    
    std::promise<void> promise;
    auto future = promise.get_future();
    
    client->sendRequest(req, [&](ReqResult result, const HttpResponsePtr &response) {
        try {
            ASSERT_EQ(result, ReqResult::Ok);
            ASSERT_EQ(response->getStatusCode(), k400BadRequest);
            
            auto json = response->getJsonObject();
            ASSERT_TRUE(json);
            ASSERT_TRUE((*json)["error"].asBool());
            ASSERT_TRUE((*json)["message"].asString().find("Missing required field: name") != std::string::npos);
            
            promise.set_value();
        } catch (...) {
            promise.set_exception(std::current_exception());
        }
    });
    
    auto status = future.wait_for(std::chrono::seconds(10));
    ASSERT_EQ(status, std::future_status::ready);
    future.get();
}

TEST_F(ValidationMiddlewareTest, InvalidProductCreationInvalidPrice) {
    Json::Value product;
    product["sku"] = "TEST003";
    product["name"] = "Test Product";
    product["description"] = "A test product description";
    product["unit_price"] = -10.99; // Invalid negative price
    product["quantity_in_stock"] = 100;
    product["reorder_threshold"] = 10;
    
    Json::StreamWriterBuilder builder;
    std::string jsonString = Json::writeString(builder, product);
    
    auto client = HttpClient::newHttpClient("http://127.0.0.1:7777");
    auto req = HttpRequest::newHttpRequest();
    req->setMethod(Post);
    req->setPath("/api/products");
    req->setBody(jsonString);
    req->setContentTypeCode(CT_APPLICATION_JSON);
    
    std::promise<void> promise;
    auto future = promise.get_future();
    
    client->sendRequest(req, [&](ReqResult result, const HttpResponsePtr &response) {
        try {
            ASSERT_EQ(result, ReqResult::Ok);
            ASSERT_EQ(response->getStatusCode(), k400BadRequest);
            
            auto json = response->getJsonObject();
            ASSERT_TRUE(json);
            ASSERT_TRUE((*json)["error"].asBool());
            
            promise.set_value();
        } catch (...) {
            promise.set_exception(std::current_exception());
        }
    });
    
    auto status = future.wait_for(std::chrono::seconds(10));
    ASSERT_EQ(status, std::future_status::ready);
    future.get();
}

TEST_F(ValidationMiddlewareTest, InvalidProductId) {
    auto client = HttpClient::newHttpClient("http://127.0.0.1:7777");
    auto req = HttpRequest::newHttpRequest();
    req->setMethod(Get);
    req->setPath("/api/products/abc"); // Invalid ID format
    
    std::promise<void> promise;
    auto future = promise.get_future();
    
    client->sendRequest(req, [&](ReqResult result, const HttpResponsePtr &response) {
        try {
            ASSERT_EQ(result, ReqResult::Ok);
            ASSERT_EQ(response->getStatusCode(), k400BadRequest);
            
            auto json = response->getJsonObject();
            ASSERT_TRUE(json);
            ASSERT_TRUE((*json)["error"].asBool());
            ASSERT_TRUE((*json)["message"].asString().find("Invalid ID") != std::string::npos);
            
            promise.set_value();
        } catch (...) {
            promise.set_exception(std::current_exception());
        }
    });
    
    auto status = future.wait_for(std::chrono::seconds(10));
    ASSERT_EQ(status, std::future_status::ready);
    future.get();
}

TEST_F(ValidationMiddlewareTest, EmptyRequestBody) {
    auto client = HttpClient::newHttpClient("http://127.0.0.1:7777");
    auto req = HttpRequest::newHttpRequest();
    req->setMethod(Post);
    req->setPath("/api/products");
    req->setBody(""); // Empty body
    req->setContentTypeCode(CT_APPLICATION_JSON);
    
    std::promise<void> promise;
    auto future = promise.get_future();
    
    client->sendRequest(req, [&](ReqResult result, const HttpResponsePtr &response) {
        try {
            ASSERT_EQ(result, ReqResult::Ok);
            ASSERT_EQ(response->getStatusCode(), k400BadRequest);
            
            auto json = response->getJsonObject();
            ASSERT_TRUE(json);
            ASSERT_TRUE((*json)["error"].asBool());
            ASSERT_TRUE((*json)["message"].asString().find("Request body cannot be empty") != std::string::npos);
            
            promise.set_value();
        } catch (...) {
            promise.set_exception(std::current_exception());
        }
    });
    
    auto status = future.wait_for(std::chrono::seconds(10));
    ASSERT_EQ(status, std::future_status::ready);
    future.get();
}

TEST_F(ValidationMiddlewareTest, InvalidJsonFormat) {
    auto client = HttpClient::newHttpClient("http://127.0.0.1:7777");
    auto req = HttpRequest::newHttpRequest();
    req->setMethod(Post);
    req->setPath("/api/products");
    req->setBody("{invalid json}"); // Malformed JSON
    req->setContentTypeCode(CT_APPLICATION_JSON);
    
    std::promise<void> promise;
    auto future = promise.get_future();
    
    client->sendRequest(req, [&](ReqResult result, const HttpResponsePtr &response) {
        try {
            ASSERT_EQ(result, ReqResult::Ok);
            ASSERT_EQ(response->getStatusCode(), k400BadRequest);
            
            auto json = response->getJsonObject();
            ASSERT_TRUE(json);
            ASSERT_TRUE((*json)["error"].asBool());
            ASSERT_TRUE((*json)["message"].asString().find("Invalid JSON format") != std::string::npos);
            
            promise.set_value();
        } catch (...) {
            promise.set_exception(std::current_exception());
        }
    });
    
    auto status = future.wait_for(std::chrono::seconds(10));
    ASSERT_EQ(status, std::future_status::ready);
    future.get();
}