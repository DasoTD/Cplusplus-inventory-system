#include "validation.h"
#include <json/json.h>
#include <vector>
#include <string>

drogon::HttpResponsePtr validateProductRequest(const drogon::HttpRequestPtr& req) {
    const auto method = req->getMethod();
    const auto path = req->getPath();
    
    if ((method == drogon::Post || method == drogon::Put) && path.find("/api/products") == 0) {
        LOG_INFO << "Validating " << req->getMethodString() << " " << path;
        
        const auto bodyView = req->getBody();
        std::string body(bodyView.data(), bodyView.size());
        
        if (body.empty()) {
            LOG_INFO << "Empty request body, returning 400";
            Json::Value response;
            response["error"] = true;
            response["message"] = "Request body cannot be empty";
            auto httpResp = drogon::HttpResponse::newHttpJsonResponse(response);
            httpResp->setStatusCode(drogon::k400BadRequest);
            return httpResp;
        }
        
        Json::Value json;
        Json::Reader reader;
        if (!reader.parse(body, json)) {
            LOG_INFO << "Invalid JSON format, returning 400";
            Json::Value response;
            response["error"] = true;
            response["message"] = "Invalid JSON format";
            auto httpResp = drogon::HttpResponse::newHttpJsonResponse(response);
            httpResp->setStatusCode(drogon::k400BadRequest);
            return httpResp;
        }
        
        // Validate required fields for POST to /api/products
        if (method == drogon::Post && path == "/api/products") {
            const std::vector<std::string> requiredFields = {"sku", "name", "unit_price", "quantity_in_stock", "reorder_threshold"};
            
            for (const auto& field : requiredFields) {
                if (!json.isMember(field)) {
                    LOG_INFO << "Missing required field: " << field;
                    Json::Value response;
                    response["error"] = true;
                    response["message"] = "Missing required field: " + field;
                    auto httpResp = drogon::HttpResponse::newHttpJsonResponse(response);
                    httpResp->setStatusCode(drogon::k400BadRequest);
                    return httpResp;
                }
            }
            
            // Validate unit_price is positive
            if (json.isMember("unit_price") && json["unit_price"].isDouble()) {
                double price = json["unit_price"].asDouble();
                if (price < 0) {
                    LOG_INFO << "Invalid negative price: " << price;
                    Json::Value response;
                    response["error"] = true;
                    response["message"] = "Unit price must be a positive number";
                    auto httpResp = drogon::HttpResponse::newHttpJsonResponse(response);
                    httpResp->setStatusCode(drogon::k400BadRequest);
                    return httpResp;
                }
            }
        }
        
        LOG_INFO << "Validation passed";
    }
    
    return nullptr; // No error, continue processing
}