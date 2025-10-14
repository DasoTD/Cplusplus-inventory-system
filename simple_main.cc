#include <drogon/drogon.h>
#include <drogon/orm/Exception.h>
#include <drogon/orm/Mapper.h>
#include <json/json.h>
#include "models/Products.h"

int main() {
    // Set HTTP listener address and port
    drogon::app().addListener("0.0.0.0", 8888);

    // Load config file for database configuration
    drogon::app().loadConfigFile("config.json");

    LOG_INFO << "Starting inventory system server on port 8888";

    // GET all products
    drogon::app().registerHandler(
        "/api/products", [](const drogon::HttpRequestPtr& req,
                            std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
            if (req->getMethod() == drogon::Get) {
                try {
                    auto dbClient = drogon::app().getDbClient();
                    auto mapper = drogon::orm::Mapper<drogon_model::sqlite3::Products>(dbClient);

                    mapper.findAll(
                        [callback](std::vector<drogon_model::sqlite3::Products> products) {
                            Json::Value response(Json::arrayValue);
                            for (const auto& product : products) {
                                response.append(product.toJson());
                            }
                            auto resp = drogon::HttpResponse::newHttpJsonResponse(response);
                            resp->setStatusCode(drogon::k200OK);
                            callback(resp);
                        },
                        [callback](const drogon::orm::DrogonDbException& e) {
                            Json::Value error;
                            error["error"] = "Failed to retrieve products";
                            error["message"] = e.base().what();
                            auto resp = drogon::HttpResponse::newHttpJsonResponse(error);
                            resp->setStatusCode(drogon::k500InternalServerError);
                            callback(resp);
                        });
                } catch (const std::exception& e) {
                    Json::Value error;
                    error["error"] = "Database connection failed";
                    error["message"] = e.what();
                    auto resp = drogon::HttpResponse::newHttpJsonResponse(error);
                    resp->setStatusCode(drogon::k500InternalServerError);
                    callback(resp);
                }
            }
        });

    // POST new product
    drogon::app().registerHandler(
        "/api/products", [](const drogon::HttpRequestPtr& req,
                            std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
            if (req->getMethod() == drogon::Post) {
                try {
                    auto json = req->getJsonObject();
                    if (!json) {
                        Json::Value error;
                        error["error"] = "Invalid JSON";
                        auto resp = drogon::HttpResponse::newHttpJsonResponse(error);
                        resp->setStatusCode(drogon::k400BadRequest);
                        callback(resp);
                        return;
                    }

                    auto dbClient = drogon::app().getDbClient();
                    auto mapper = drogon::orm::Mapper<drogon_model::sqlite3::Products>(dbClient);

                    drogon_model::sqlite3::Products product(*json);

                    mapper.insert(
                        product,
                        [callback](drogon_model::sqlite3::Products newProduct) {
                            Json::Value response = newProduct.toJson();
                            auto resp = drogon::HttpResponse::newHttpJsonResponse(response);
                            resp->setStatusCode(drogon::k201Created);
                            callback(resp);
                        },
                        [callback](const drogon::orm::DrogonDbException& e) {
                            Json::Value error;
                            error["error"] = "Failed to create product";
                            error["message"] = e.base().what();
                            auto resp = drogon::HttpResponse::newHttpJsonResponse(error);
                            resp->setStatusCode(drogon::k500InternalServerError);
                            callback(resp);
                        });
                } catch (const std::exception& e) {
                    Json::Value error;
                    error["error"] = "Invalid product data";
                    error["message"] = e.what();
                    auto resp = drogon::HttpResponse::newHttpJsonResponse(error);
                    resp->setStatusCode(drogon::k400BadRequest);
                    callback(resp);
                }
            }
        });

    // Test endpoint
    drogon::app().registerHandler(
        "/test", [](const drogon::HttpRequestPtr& req,
                    std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
            Json::Value response;
            response["message"] = "Simple server is working!";
            response["timestamp"] = std::time(nullptr);
            auto resp = drogon::HttpResponse::newHttpJsonResponse(response);
            callback(resp);
        });

    // Set up CORS for API testing
    drogon::app().registerPostHandlingAdvice(
        [](const drogon::HttpRequestPtr&, const drogon::HttpResponsePtr& resp) {
            resp->addHeader("Access-Control-Allow-Origin", "*");
            resp->addHeader("Access-Control-Allow-Methods", "GET,POST,PUT,DELETE,OPTIONS");
            resp->addHeader("Access-Control-Allow-Headers", "Content-Type");
        });

    // Run HTTP framework,the method will block in the internal event loop
    drogon::app().run();
    return 0;
}