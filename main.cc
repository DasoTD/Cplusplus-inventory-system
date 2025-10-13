/*
// ==========================================
// COMMENTED OUT - CSP/WEB INTERFACE VERSION
// Uncomment this section to enable web interface
// ==========================================

#include <drogon/drogon.h>
#include <json/json.h>
#include "models/Products.h"
#include "models/PurchaseOrders.h"
#include <drogon/orm/Mapper.h>
#include <drogon/orm/Exception.h>

int main() {
    //Set HTTP listener address and port
    drogon::app().addListener("0.0.0.0", 7777);
    
    //Load config file for database configuration
    drogon::app().loadConfigFile("config.json");
    
    LOG_INFO << "Starting inventory system with web interface on port 7777";
    
    // Serve the main HTML page directly
    drogon::app().registerHandler("/", [](const drogon::HttpRequestPtr &req,
                                         std::function<void(const drogon::HttpResponsePtr &)> &&callback) {
        std::string html = "<!DOCTYPE html>"
"<html>"
"<head>"
"    <title>Inventory Management</title>"
"    <style>"
"        table { border-collapse: collapse; width: 100%; }"
"        th, td { border: 1px solid black; padding: 8px; text-align: left; }"
"        button { margin: 5px; }"
"    </style>"
"</head>"
"<body>"
"    <h1>Inventory Management System</h1>"
"    <h2>Products</h2>"
"    <table id='productsTable'>"
"        <tr><th>ID</th><th>SKU</th><th>Name</th><th>Stock</th><th>Threshold</th><th>Action</th></tr>"
"    </table>"
"    <h2>Purchase Orders</h2>"
"    <table id='ordersTable'>"
"        <tr><th>ID</th><th>Product ID</th><th>Quantity</th><th>Order Date</th><th>Status</th><th>Action</th></tr>"
"    </table>"
"    <script>"
"        fetch('/api/products')"
"            .then(response => response.json())"
"            .then(data => {"
"                const table = document.getElementById('productsTable');"
"                data.forEach(product => {"
"                    const row = table.insertRow();"
"                    row.innerHTML = '<td>' + product.product_id + '</td>' +"
"                                   '<td>' + product.sku + '</td>' +"
"                                   '<td>' + product.name + '</td>' +"
"                                   '<td>' + product.quantity_in_stock + '</td>' +"
"                                   '<td>' + product.reorder_threshold + '</td>' +"
"                                   '<td><input type=\"number\" id=\"qty' + product.product_id + '\" placeholder=\"New stock\">' +"
"                                   '<button onclick=\"updateStock(' + product.product_id + ')\">Update</button></td>';"
"                });"
"            });"
"        fetch('/api/orders')"
"            .then(response => response.json())"
"            .then(data => {"
"                const table = document.getElementById('ordersTable');"
"                data.forEach(order => {"
"                    const row = table.insertRow();"
"                    row.innerHTML = '<td>' + order.order_id + '</td>' +"
"                                   '<td>' + order.product_id + '</td>' +"
"                                   '<td>' + order.quantity_ordered + '</td>' +"
"                                   '<td>' + order.order_date + '</td>' +"
"                                   '<td>' + order.status + '</td>' +"
"                                   '<td>' + (order.status === 'PENDING' ? '<button onclick=\"completeOrder(' + order.order_id + ')\">Complete</button>' : '') + '</td>';"
"                });"
"            });"
"        function updateStock(productId) {"
"            const qty = document.getElementById('qty' + productId).value;"
"            fetch('/api/products/' + productId + '/stock', {"
"                method: 'POST',"
"                headers: { 'Content-Type': 'application/json' },"
"                body: JSON.stringify({ quantity: parseInt(qty) })"
"            }).then(() => location.reload());"
"        }"
"        function completeOrder(orderId) {"
"            fetch('/api/orders/' + orderId + '/complete', {"
"                method: 'POST'"
"            }).then(() => location.reload());"
"        }"
"    </script>"
"</body>"
"</html>";
        auto resp = drogon::HttpResponse::newHttpResponse();
        resp->setBody(html);
        resp->setContentTypeCode(drogon::CT_TEXT_HTML);
        callback(resp);
    });
    
    // API Endpoints for the web interface
    
    // GET all products
    drogon::app().registerHandler("/api/products", [](const drogon::HttpRequestPtr &req,
                                                     std::function<void(const drogon::HttpResponsePtr &)> &&callback) {
        if (req->getMethod() == drogon::Get) {
            try {
                auto dbClient = drogon::app().getDbClient();
                auto mapper = drogon::orm::Mapper<drogon_model::sqlite3::Products>(dbClient);
                
                mapper.findAll([callback](std::vector<drogon_model::sqlite3::Products> products) {
                    Json::Value response(Json::arrayValue);
                    for (const auto &product : products) {
                        response.append(product.toJson());
                    }
                    auto resp = drogon::HttpResponse::newHttpJsonResponse(response);
                    resp->setStatusCode(drogon::k200OK);
                    callback(resp);
                }, [callback](const drogon::orm::DrogonDbException &e) {
                    Json::Value error;
                    error["error"] = "Failed to retrieve products";
                    error["message"] = e.base().what();
                    auto resp = drogon::HttpResponse::newHttpJsonResponse(error);
                    resp->setStatusCode(drogon::k500InternalServerError);
                    callback(resp);
                });
            } catch (const std::exception &e) {
                Json::Value error;
                error["error"] = "Database connection failed";
                error["message"] = e.what();
                auto resp = drogon::HttpResponse::newHttpJsonResponse(error);
                resp->setStatusCode(drogon::k500InternalServerError);
                callback(resp);
            }
        }
    });
    
    // GET all purchase orders
    drogon::app().registerHandler("/api/orders", [](const drogon::HttpRequestPtr &req,
                                                   std::function<void(const drogon::HttpResponsePtr &)> &&callback) {
        try {
            auto dbClient = drogon::app().getDbClient();
            auto mapper = drogon::orm::Mapper<drogon_model::sqlite3::PurchaseOrders>(dbClient);
            
            mapper.findAll([callback](std::vector<drogon_model::sqlite3::PurchaseOrders> orders) {
                Json::Value response(Json::arrayValue);
                for (const auto &order : orders) {
                    response.append(order.toJson());
                }
                auto resp = drogon::HttpResponse::newHttpJsonResponse(response);
                resp->setStatusCode(drogon::k200OK);
                callback(resp);
            }, [callback](const drogon::orm::DrogonDbException &e) {
                Json::Value error;
                error["error"] = "Failed to retrieve orders";
                error["message"] = e.base().what();
                auto resp = drogon::HttpResponse::newHttpJsonResponse(error);
                resp->setStatusCode(drogon::k500InternalServerError);
                callback(resp);
            });
        } catch (const std::exception &e) {
            Json::Value error;
            error["error"] = "Database connection failed";
            error["message"] = e.what();
            auto resp = drogon::HttpResponse::newHttpJsonResponse(error);
            resp->setStatusCode(drogon::k500InternalServerError);
            callback(resp);
        }
    });
    
    // Update product stock
    drogon::app().registerHandler("/api/products/{product_id}/stock", [](const drogon::HttpRequestPtr &req,
                                                                        std::function<void(const drogon::HttpResponsePtr &)> &&callback,
                                                                        std::string &&product_id) {
        if (req->getMethod() == drogon::Post) {
            try {
                auto json = req->getJsonObject();
                if (!json || !json->isMember("quantity")) {
                    Json::Value error;
                    error["error"] = "Invalid JSON or missing quantity";
                    auto resp = drogon::HttpResponse::newHttpJsonResponse(error);
                    resp->setStatusCode(drogon::k400BadRequest);
                    callback(resp);
                    return;
                }

                auto dbClient = drogon::app().getDbClient();
                auto mapper = drogon::orm::Mapper<drogon_model::sqlite3::Products>(dbClient);
                
                int64_t id = std::stoll(product_id);
                int newQuantity = (*json)["quantity"].asInt();
                
                mapper.findByPrimaryKey(id, [callback, dbClient, newQuantity](drogon_model::sqlite3::Products product) {
                    product.setQuantityInStock(newQuantity);
                    auto updateMapper = drogon::orm::Mapper<drogon_model::sqlite3::Products>(dbClient);
                    updateMapper.update(product, [callback](const size_t count) {
                        Json::Value response;
                        response["success"] = true;
                        response["updated"] = static_cast<int>(count);
                        auto resp = drogon::HttpResponse::newHttpJsonResponse(response);
                        callback(resp);
                    }, [callback](const drogon::orm::DrogonDbException &e) {
                        Json::Value error;
                        error["error"] = "Failed to update stock";
                        error["message"] = e.base().what();
                        auto resp = drogon::HttpResponse::newHttpJsonResponse(error);
                        resp->setStatusCode(drogon::k500InternalServerError);
                        callback(resp);
                    });
                }, [callback](const drogon::orm::DrogonDbException &e) {
                    Json::Value error;
                    error["error"] = "Product not found";
                    auto resp = drogon::HttpResponse::newHttpJsonResponse(error);
                    resp->setStatusCode(drogon::k404NotFound);
                    callback(resp);
                });
            } catch (const std::exception &e) {
                Json::Value error;
                error["error"] = "Invalid product ID or data";
                error["message"] = e.what();
                auto resp = drogon::HttpResponse::newHttpJsonResponse(error);
                resp->setStatusCode(drogon::k400BadRequest);
                callback(resp);
            }
        }
    });
    
    // Complete purchase order
    drogon::app().registerHandler("/api/orders/{order_id}/complete", [](const drogon::HttpRequestPtr &req,
                                                                       std::function<void(const drogon::HttpResponsePtr &)> &&callback,
                                                                       std::string &&order_id) {
        if (req->getMethod() == drogon::Post) {
            try {
                auto dbClient = drogon::app().getDbClient();
                auto orderMapper = drogon::orm::Mapper<drogon_model::sqlite3::PurchaseOrders>(dbClient);
                auto productMapper = drogon::orm::Mapper<drogon_model::sqlite3::Products>(dbClient);
                
                int64_t id = std::stoll(order_id);
                
                orderMapper.findByPrimaryKey(id, [callback, dbClient](drogon_model::sqlite3::PurchaseOrders order) {
                    if (order.getValueOfStatus() == "COMPLETED") {
                        Json::Value error;
                        error["error"] = "Order already completed";
                        auto resp = drogon::HttpResponse::newHttpJsonResponse(error);
                        resp->setStatusCode(drogon::k400BadRequest);
                        callback(resp);
                        return;
                    }
                    
                    // Update order status to completed (simplified - just return success)
                    order.setStatus("COMPLETED");
                    Json::Value response;
                    response["success"] = true;
                    response["message"] = "Order marked as completed";
                    auto resp = drogon::HttpResponse::newHttpJsonResponse(response);
                    callback(resp);
                }, [callback](const drogon::orm::DrogonDbException &e) {
                    Json::Value error;
                    error["error"] = "Order not found";
                    auto resp = drogon::HttpResponse::newHttpJsonResponse(error);
                    resp->setStatusCode(drogon::k404NotFound);
                    callback(resp);
                });
            } catch (const std::exception &e) {
                Json::Value error;
                error["error"] = "Invalid order ID";
                error["message"] = e.what();
                auto resp = drogon::HttpResponse::newHttpJsonResponse(error);
                resp->setStatusCode(drogon::k400BadRequest);
                callback(resp);
            }
        }
    });
    
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

// ==========================================
// END OF COMMENTED CSP/WEB INTERFACE VERSION
// ==========================================
*/

// ==========================================
// CLEAN ARCHITECTURE VERSION - PROPER CONTROLLERS
// Controllers handle all business logic, main.cc only handles setup
// ==========================================

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

// ==========================================
// END OF ORIGINAL WORKING API CODE
// ==========================================
