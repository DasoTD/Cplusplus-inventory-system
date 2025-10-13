#include <drogon/drogon.h>
#include <json/json.h>
// Include controllers to ensure they are compiled and auto-registered
#include "controllers/ProductsController.h"

int main() {
    //Set HTTP listener address and port
    drogon::app().addListener("0.0.0.0", 7777);
    
    //Load config file for database configuration
    drogon::app().loadConfigFile("config.json");
    
    LOG_INFO << "Starting inventory system server on port 7777 with proper controller architecture";
    
    // Create ProductsController instance for API routing
    auto productsController = std::make_shared<ProductsController>();
    
    // Map /api/products routes to ProductsController methods
    drogon::app().registerHandler("/api/products", [productsController](const drogon::HttpRequestPtr &req,
                                                                        std::function<void(const drogon::HttpResponsePtr &)> &&callback) {
        if (req->getMethod() == drogon::Get) {
            productsController->get(req, std::move(callback));
        } else if (req->getMethod() == drogon::Post) {
            productsController->create(req, std::move(callback));
        } else {
            auto resp = drogon::HttpResponse::newHttpResponse();
            resp->setStatusCode(drogon::k405MethodNotAllowed);
            callback(resp);
        }
    });
    
    // Map /api/products/{id} routes to ProductsController methods
    drogon::app().registerHandler("/api/products/{id}", [productsController](const drogon::HttpRequestPtr &req,
                                                                             std::function<void(const drogon::HttpResponsePtr &)> &&callback,
                                                                             std::string &&id) {
        if (req->getMethod() == drogon::Get) {
            productsController->getOne(req, std::move(callback), std::move(id));
        } else if (req->getMethod() == drogon::Put) {
            productsController->updateOne(req, std::move(callback), std::move(id));
        } else if (req->getMethod() == drogon::Delete) {
            productsController->deleteOne(req, std::move(callback), std::move(id));
        } else {
            auto resp = drogon::HttpResponse::newHttpResponse();
            resp->setStatusCode(drogon::k405MethodNotAllowed);
            callback(resp);
        }
    });
    
    // Simple health check endpoint
    drogon::app().registerHandler("/health", [](const drogon::HttpRequestPtr &req,
                                               std::function<void(const drogon::HttpResponsePtr &)> &&callback) {
        Json::Value response;
        response["status"] = "healthy";
        response["service"] = "inventory-system";
        response["timestamp"] = std::time(nullptr);
        auto resp = drogon::HttpResponse::newHttpJsonResponse(response);
        callback(resp);
    });
    
    // API documentation endpoint
    drogon::app().registerHandler("/api", [](const drogon::HttpRequestPtr &req,
                                            std::function<void(const drogon::HttpResponsePtr &)> &&callback) {
        Json::Value response;
        response["service"] = "Inventory Management System";
        response["version"] = "1.0.0";
        Json::Value endpoints(Json::arrayValue);
        endpoints.append("GET /api/products - List all products");
        endpoints.append("POST /api/products - Create new product");
        endpoints.append("GET /api/products/{id} - Get product by ID");
        endpoints.append("PUT /api/products/{id} - Update product");
        endpoints.append("DELETE /api/products/{id} - Delete product");
        endpoints.append("GET /health - Health check");
        response["endpoints"] = endpoints;
        auto resp = drogon::HttpResponse::newHttpJsonResponse(response);
        callback(resp);
    });
    
    LOG_INFO << "Controller architecture initialized:";
    LOG_INFO << "  - ProductsController: Business logic in controllers/ProductsController.cc";
    LOG_INFO << "  - API Routes: /api/products/* mapped to ProductsController methods";
    LOG_INFO << "  - Clean separation: main.cc handles routing, controllers handle business logic";
    
    //Set up CORS for API testing  
    drogon::app().registerPostHandlingAdvice([](const drogon::HttpRequestPtr &, const drogon::HttpResponsePtr &resp) {
        resp->addHeader("Access-Control-Allow-Origin", "*");
        resp->addHeader("Access-Control-Allow-Methods", "GET,POST,PUT,DELETE,OPTIONS"); 
        resp->addHeader("Access-Control-Allow-Headers", "Content-Type");
    });
    
    //Run HTTP framework,the method will block in the internal event loop
    drogon::app().run();
    return 0;
}
