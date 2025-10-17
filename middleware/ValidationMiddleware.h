#pragma once

#include <drogon/HttpMiddleware.h>
#include <drogon/HttpRequest.h>
#include <drogon/HttpResponse.h>
#include <json/json.h>
#include <regex>
#include <string>
#include <unordered_set>

using namespace drogon;

class ValidationMiddleware : public HttpMiddleware<ValidationMiddleware>
{
public:
    ValidationMiddleware() = default;
    
    void invoke(const HttpRequestPtr &req,
                MiddlewareNextCallback &&nextCb,
                MiddlewareCallback &&mcb) override;

private:
    // Validation helper methods
    static bool validateJson(const std::string& body, Json::Value& jsonOut);
    static bool validateProductData(const Json::Value& json, std::string& error);
    static bool validateProductUpdate(const Json::Value& json, std::string& error);
    static bool validateId(const std::string& id, std::string& error);
    static bool validateString(const std::string& str, const std::string& fieldName,
                             size_t minLen = 1, size_t maxLen = 255);
    static bool validateNumber(const Json::Value& value, const std::string& fieldName,
                             double min = 0.0, double max = 1000000.0);
    static bool validatePrice(const Json::Value& value, const std::string& fieldName);
    static bool validateQuantity(const Json::Value& value, const std::string& fieldName);
    
    // HTTP method validation
    static bool requiresValidation(const HttpMethod& method, const std::string& path);
    static bool isCreateEndpoint(const std::string& path);
    static bool isUpdateEndpoint(const std::string& path);
    static bool isIdEndpoint(const std::string& path);

    // Response helpers
    static HttpResponsePtr createErrorResponse(const std::string& message,
                                             HttpStatusCode code = k400BadRequest);
};