#include "ValidationMiddleware.h"
#include <drogon/utils/Utilities.h>
#include <regex>
#include <algorithm>
#include <cmath>
#include <cctype>
#include <ctime>

void ValidationMiddleware::invoke(const HttpRequestPtr& req,
                                MiddlewareNextCallback&& nextCb,
                                MiddlewareCallback&& mcb)
{
    const auto method = req->getMethod();
    const auto path = req->getPath();
    
    // Debug: Log all requests to see if middleware is being called
    LOG_INFO << "ValidationMiddleware: " << req->getMethodString() << " " << path;
    
    // Skip validation for GET requests and health check
    if (!requiresValidation(method, path)) {
        LOG_INFO << "ValidationMiddleware: Skipping validation for " << req->getMethodString() << " " << path;
        nextCb([mcb = std::move(mcb)](const HttpResponsePtr &resp) {
            mcb(resp);
        });
        return;
    }
    
    LOG_INFO << "ValidationMiddleware: Validating " << req->getMethodString() << " " << path;
    
    std::string error;
    
    // Validate ID in URL path for endpoints that require it
    if (isIdEndpoint(path)) {
        // Extract ID from path (e.g., /api/products/123 -> 123)
        auto pathParts = drogon::utils::splitString(path, "/");
        if (pathParts.size() >= 3) {
            const std::string &id = pathParts[pathParts.size() - 1];
            if (!validateId(id, error)) {
                auto resp = createErrorResponse("Invalid ID: " + error);
                mcb(resp);
                return;
            }
        }
    }
    
    // Validate JSON body for POST/PUT requests
    if (method == drogon::Post || method == drogon::Put) {
        LOG_INFO << "ValidationMiddleware: Processing POST/PUT request";
        
        // req->getBody() returns a std::string_view; construct an std::string explicitly
        const auto bodyView = req->getBody();
        std::string body(bodyView.data(), bodyView.size());
        
        LOG_INFO << "ValidationMiddleware: Request body: " << body;
        
        if (body.empty()) {
            LOG_INFO << "ValidationMiddleware: Request body is empty, returning 400";
            auto resp = createErrorResponse("Request body cannot be empty");
            mcb(resp);
            return;
        }
        
        Json::Value json;
        if (!validateJson(body, json)) {
            LOG_INFO << "ValidationMiddleware: Invalid JSON format, returning 400";
            auto resp = createErrorResponse("Invalid JSON format");
            mcb(resp);
            return;
        }
        
        // Validate product data based on endpoint
        bool isValid = false;
        if (isCreateEndpoint(path)) {
            LOG_INFO << "ValidationMiddleware: Validating product creation data";
            isValid = validateProductData(json, error);
        } else if (isUpdateEndpoint(path)) {
            LOG_INFO << "ValidationMiddleware: Validating product update data";
            isValid = validateProductUpdate(json, error);
        }
        
        if (!isValid) {
            LOG_INFO << "ValidationMiddleware: Validation failed: " << error;
            auto resp = createErrorResponse("Validation error: " + error);
            mcb(resp);
            return;
        }
        
        LOG_INFO << "ValidationMiddleware: Validation passed, proceeding to next handler";
        
        // Store validated JSON in request attributes for controllers to use
        req->getAttributes()->insert("validated_json", json);
    }
    
    // Continue to next middleware/controller
    nextCb([mcb = std::move(mcb)](const HttpResponsePtr &resp) {
        mcb(resp);
    });
}

bool ValidationMiddleware::validateJson(const std::string &body, Json::Value &jsonOut)
{
    Json::CharReaderBuilder builder;
    std::unique_ptr<Json::CharReader> reader(builder.newCharReader());
    std::string errors;
    
    bool success = reader->parse(body.c_str(), body.c_str() + body.length(), &jsonOut, &errors);
    
    return success && jsonOut.isObject();
}

bool ValidationMiddleware::validateProductData(const Json::Value &json, std::string &error)
{
    // Required fields for creating a product
    const std::vector<std::string> requiredFields = {"sku", "name", "unit_price", "quantity_in_stock", "reorder_threshold"};

    for (const auto& field : requiredFields) {
        if (!json.isMember(field)) {
            error = "Missing required field: " + field;
            return false;
        }
    }
    
    // Validate SKU
    if (!json["sku"].isString() || !validateString(json["sku"].asString(), "sku", 1, 50)) {
        error = "SKU must be a non-empty string with max 50 characters";
        return false;
    }
    
    // Validate name
    if (!json["name"].isString() || !validateString(json["name"].asString(), "name", 1, 100)) {
        error = "Name must be a non-empty string with max 100 characters";
        return false;
    }
    
    // Validate description (optional)
    if (json.isMember("description") && json["description"].isString()) {
        if (!validateString(json["description"].asString(), "description", 0, 500)) {
            error = "Description must be a string with max 500 characters";
            return false;
        }
    }
    
    // Validate unit_price
    if (!validatePrice(json["unit_price"], "unit_price")) {
        error = "Unit price must be a positive number";
        return false;
    }
    
    // Validate quantity_in_stock
    if (!validateQuantity(json["quantity_in_stock"], "quantity_in_stock")) {
        error = "Quantity in stock must be a non-negative integer";
        return false;
    }
    
    // Validate reorder_threshold
    if (!validateQuantity(json["reorder_threshold"], "reorder_threshold")) {
        error = "Reorder threshold must be a non-negative integer";
        return false;
    }
    
    return true;
}

bool ValidationMiddleware::validateProductUpdate(const Json::Value &json, std::string &error)
{
    // For updates, fields are optional but must be valid if present
    
    if (json.isMember("name")) {
        if (!json["name"].isString() || !validateString(json["name"].asString(), "name", 1, 100)) {
            error = "Name must be a non-empty string with max 100 characters";
            return false;
        }
    }
    
    if (json.isMember("description")) {
        if (!json["description"].isString() || !validateString(json["description"].asString(), "description", 1, 500)) {
            error = "Description must be a non-empty string with max 500 characters";
            return false;
        }
    }
    
    if (json.isMember("unit_price")) {
        if (!validatePrice(json["unit_price"], "unit_price")) {
            error = "Unit price must be a positive number";
            return false;
        }
    }
    
    if (json.isMember("quantity_in_stock")) {
        if (!validateQuantity(json["quantity_in_stock"], "quantity_in_stock")) {
            error = "Quantity in stock must be a non-negative integer";
            return false;
        }
    }
    
    // Ensure at least one field is provided for update
    if (!json.isMember("name") && !json.isMember("description") && 
        !json.isMember("price") && !json.isMember("quantity")) {
        error = "At least one field must be provided for update";
        return false;
    }
    
    return true;
}

bool ValidationMiddleware::validateId(const std::string &id, std::string &error)
{
    if (id.empty()) {
        error = "ID cannot be empty";
        return false;
    }
    
    // Check if ID is a positive integer
    try {
        std::regex idPattern("^[1-9]\\d*$");
        if (!std::regex_match(id, idPattern)) {
            error = "ID must be a positive integer";
            return false;
        }
    } catch (const std::regex_error& e) {
        error = "ID validation pattern error";
        return false;
    }
    
    // Check reasonable length (prevent extremely large numbers)
    if (id.length() > 10) {
        error = "ID is too large";
        return false;
    }
    
    return true;
}

bool ValidationMiddleware::validateString(const std::string &str, const std::string &fieldName, 
                                        size_t minLen, size_t maxLen)
{
    if (str.length() < minLen || str.length() > maxLen) {
        return false;
    }
    
    // Check for basic sanitization - no control characters
    for (char c : str) {
        if (std::iscntrl(static_cast<unsigned char>(c)) && c != '\n' && c != '\r' && c != '\t') {
            return false;
        }
    }
    
    return true;
}

bool ValidationMiddleware::validateNumber(const Json::Value &value, const std::string &fieldName,
                                        double min, double max)
{
    if (!value.isNumeric()) {
        return false;
    }
    
    double num = value.asDouble();
    return num >= min && num <= max && std::isfinite(num);
}

bool ValidationMiddleware::validatePrice(const Json::Value& value, const std::string& fieldName)
{
    if (!validateNumber(value, fieldName, 0.01, 999999.99)) {
        return false;
    }
    
    // Check decimal places (max 2)
    double price = value.asDouble();
    double rounded = std::round(price * 100) / 100.0;
    return std::abs(price - rounded) < 1e-9;
}

bool ValidationMiddleware::validateQuantity(const Json::Value& value, const std::string& fieldName)
{
    if (!value.isInt()) {
        return false;
    }
    
    int quantity = value.asInt();
    return quantity >= 0 && quantity <= 1000000;
}

bool ValidationMiddleware::requiresValidation(const drogon::HttpMethod &method, const std::string &path)
{
    // Skip validation for GET requests, OPTIONS, and health check
    if (method == drogon::Get || method == drogon::Options) {
        return false;
    }
    
    if (path == "/health" || path == "/" || path == "/api/health") {
        return false;
    }
    
    // Validate POST, PUT, DELETE requests to API endpoints
    return path.find("/api/") == 0;
}

bool ValidationMiddleware::isCreateEndpoint(const std::string &path)
{
    return path == "/api/products" || path == "/api/products/";
}

bool ValidationMiddleware::isUpdateEndpoint(const std::string &path)
{
    try {
        std::regex updatePattern("^/api/products/\\d+/?$");
        return std::regex_match(path, updatePattern);
    } catch (const std::regex_error& e) {
        return false;
    }
}

bool ValidationMiddleware::isIdEndpoint(const std::string &path)
{
    try {
        std::regex idPattern("^/api/products/\\d+/?$");
        return std::regex_match(path, idPattern);
    } catch (const std::regex_error& e) {
        return false;
    }
}

drogon::HttpResponsePtr ValidationMiddleware::createErrorResponse(const std::string& message,
                                                                drogon::HttpStatusCode code)
{
    Json::Value response;
    response["error"] = true;
    response["message"] = message;
    response["timestamp"] = static_cast<Json::Int64>(std::time(nullptr));
    
    auto httpResp = drogon::HttpResponse::newHttpJsonResponse(response);
    httpResp->setStatusCode(code);
    return httpResp;
}

