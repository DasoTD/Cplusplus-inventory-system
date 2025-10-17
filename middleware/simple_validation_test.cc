#include <gtest/gtest.h>
#include <cstdlib>
#include <string>
#include <iostream>
#include <sstream>
#include <chrono>

class ValidationMiddlewareTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Test server should be running with middleware enabled
    }
    
    // Generate unique SKU using timestamp
    std::string generateUniqueSKU(const std::string& prefix = "TEST") {
        auto now = std::chrono::system_clock::now();
        auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch()).count();
        return prefix + std::to_string(timestamp);
    }
    
    // Helper function to execute curl and capture output
    std::pair<int, std::string> executeCurl(const std::string& curlCommand) {
        std::string command = curlCommand + " 2>/dev/null";
        FILE* pipe = popen(command.c_str(), "r");
        if (!pipe) return {-1, ""};
        
        std::ostringstream result;
        char buffer[128];
        while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
            result << buffer;
        }
        
        int exitCode = pclose(pipe);
        return {exitCode, result.str()};
    }
    
    // Helper to get HTTP status code
    int getHttpStatusCode(const std::string& url, const std::string& method = "GET", const std::string& data = "", const std::string& contentType = "") {
        std::string curlCmd = "curl -s -o /dev/null -w '%{http_code}' ";
        
        if (method == "POST") {
            curlCmd += "-X POST ";
        }
        
        if (!contentType.empty()) {
            curlCmd += "-H 'Content-Type: " + contentType + "' ";
        }
        
        if (!data.empty()) {
            curlCmd += "-d '" + data + "' ";
        }
        
        curlCmd += url;
        
        auto result = executeCurl(curlCmd);
        if (result.first == 0) {
            return std::stoi(result.second);
        }
        return -1;
    }
};

TEST_F(ValidationMiddlewareTest, ValidProductCreation) {
    std::string sku = generateUniqueSKU("VALID");
    std::string data = R"({"sku":")" + sku + R"(","name":"Test Product","description":"A test product description","unit_price":29.99,"quantity_in_stock":100,"reorder_threshold":10})";
    
    int statusCode = getHttpStatusCode("http://127.0.0.1:7777/api/products", "POST", data, "application/json");
    
    EXPECT_EQ(statusCode, 201);
}

TEST_F(ValidationMiddlewareTest, InvalidProductCreationMissingName) {
    std::string sku = generateUniqueSKU("NONAME");
    std::string data = R"({"sku":")" + sku + R"(","description":"A test product description","unit_price":29.99,"quantity_in_stock":100,"reorder_threshold":10})";
    
    int statusCode = getHttpStatusCode("http://127.0.0.1:7777/api/products", "POST", data, "application/json");
    
    EXPECT_EQ(statusCode, 400);
}

TEST_F(ValidationMiddlewareTest, InvalidProductCreationInvalidPrice) {
    std::string sku = generateUniqueSKU("NEGPRICE");
    std::string data = R"({"sku":")" + sku + R"(","name":"Test Product","description":"A test product description","unit_price":-10.99,"quantity_in_stock":100,"reorder_threshold":10})";
    
    int statusCode = getHttpStatusCode("http://127.0.0.1:7777/api/products", "POST", data, "application/json");
    
    EXPECT_EQ(statusCode, 400);
}

TEST_F(ValidationMiddlewareTest, InvalidProductId) {
    int statusCode = getHttpStatusCode("http://127.0.0.1:7777/api/products/abc", "GET");
    
    EXPECT_EQ(statusCode, 400);
}

TEST_F(ValidationMiddlewareTest, EmptyRequestBody) {
    int statusCode = getHttpStatusCode("http://127.0.0.1:7777/api/products", "POST", "", "application/json");
    
    EXPECT_EQ(statusCode, 400);
}

TEST_F(ValidationMiddlewareTest, InvalidJsonFormat) {
    std::string data = "{invalid json}";
    
    int statusCode = getHttpStatusCode("http://127.0.0.1:7777/api/products", "POST", data, "application/json");
    
    EXPECT_EQ(statusCode, 400);
}