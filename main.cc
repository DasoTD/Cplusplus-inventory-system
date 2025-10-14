#include <drogon/drogon.h>
#include <json/json.h>
#include <mutex>
// Include controllers to ensure they are compiled and auto-registered
#include "controllers/ProductsController.h"

void initializeDatabase() {
    LOG_INFO << "Initializing database tables...";

    auto clientPtr = drogon::app().getDbClient();

    try {
        // Execute table creation queries
        // Create Products table
        std::string createProductsTable = R"(
            CREATE TABLE IF NOT EXISTS products (
                product_id INTEGER PRIMARY KEY AUTOINCREMENT,
                sku TEXT UNIQUE NOT NULL,
                name TEXT NOT NULL,
                description TEXT,
                category TEXT,
                unit_price REAL NOT NULL DEFAULT 0.0,
                quantity_in_stock INTEGER NOT NULL DEFAULT 0,
                reorder_threshold INTEGER NOT NULL DEFAULT 0,
                supplier_id INTEGER,
                warehouse_id INTEGER,
                created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
                updated_at DATETIME DEFAULT CURRENT_TIMESTAMP
            )
        )";
        clientPtr->execSqlSync(createProductsTable);

        // Create Suppliers table
        std::string createSuppliersTable = R"(
            CREATE TABLE IF NOT EXISTS supplier (
                supplier_id INTEGER PRIMARY KEY AUTOINCREMENT,
                name TEXT NOT NULL,
                contact_person TEXT,
                email TEXT,
                phone TEXT,
                address TEXT,
                created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
                updated_at DATETIME DEFAULT CURRENT_TIMESTAMP
            )
        )";
        clientPtr->execSqlSync(createSuppliersTable);

        // Create Warehouses table
        std::string createWarehousesTable = R"(
            CREATE TABLE IF NOT EXISTS warehouse (
                warehouse_id INTEGER PRIMARY KEY AUTOINCREMENT,
                name TEXT NOT NULL,
                location TEXT,
                capacity INTEGER,
                created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
                updated_at DATETIME DEFAULT CURRENT_TIMESTAMP
            )
        )";
        clientPtr->execSqlSync(createWarehousesTable);

        // Create Purchase Orders table
        std::string createPurchaseOrdersTable = R"(
            CREATE TABLE IF NOT EXISTS purchase_order (
                order_id INTEGER PRIMARY KEY AUTOINCREMENT,
                product_id INTEGER NOT NULL,
                supplier_id INTEGER NOT NULL,
                quantity_ordered INTEGER NOT NULL,
                unit_price REAL NOT NULL,
                total_price REAL NOT NULL,
                order_date DATETIME DEFAULT CURRENT_TIMESTAMP,
                expected_delivery_date DATETIME,
                actual_delivery_date DATETIME,
                status TEXT NOT NULL DEFAULT 'PENDING',
                created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
                updated_at DATETIME DEFAULT CURRENT_TIMESTAMP,
                FOREIGN KEY (product_id) REFERENCES product(product_id),
                FOREIGN KEY (supplier_id) REFERENCES supplier(supplier_id)
            )
        )";
        clientPtr->execSqlSync(createPurchaseOrdersTable);

        // Insert sample data if tables are empty
        auto result = clientPtr->execSqlSync("SELECT COUNT(*) as count FROM products");
        if (result.size() > 0 && result[0]["count"].as<int>() == 0) {
            LOG_INFO << "Inserting sample data...";

            // Insert sample supplier
            clientPtr->execSqlSync(
                "INSERT INTO supplier (name, contact_person, email, phone) VALUES ('ABC Supplies', "
                "'John Doe', 'john@abcsupplies.com', '+1234567890')");

            // Insert sample warehouse
            clientPtr->execSqlSync(
                "INSERT INTO warehouse (name, location, capacity) VALUES ('Main Warehouse', '123 "
                "Storage St', 10000)");

            // Insert sample products
            clientPtr->execSqlSync(
                "INSERT INTO products (sku, name, description, category, unit_price, "
                "quantity_in_stock, reorder_threshold, supplier_id, warehouse_id) VALUES "
                "('SKU001', 'Laptop', 'Business laptop', 'Electronics', 999.99, 50, 10, 1, 1)");
            clientPtr->execSqlSync(
                "INSERT INTO products (sku, name, description, category, unit_price, "
                "quantity_in_stock, reorder_threshold, supplier_id, warehouse_id) VALUES "
                "('SKU002', 'Mouse', 'Wireless optical mouse', 'Electronics', 29.99, 100, 20, 1, "
                "1)");
            clientPtr->execSqlSync(
                "INSERT INTO products (sku, name, description, category, unit_price, "
                "quantity_in_stock, reorder_threshold, supplier_id, warehouse_id) VALUES "
                "('SKU003', 'Keyboard', 'Mechanical keyboard', 'Electronics', 89.99, 75, 15, 1, "
                "1)");
        }

        LOG_INFO << "Database initialization completed successfully";
    } catch (const std::exception& e) {
        LOG_ERROR << "Failed to initialize database: " << e.what();
    }
}

int main() {
    // Set HTTP listener address and port
    drogon::app().addListener("0.0.0.0", 7777);

    // Load config file for database configuration
    drogon::app().loadConfigFile("config.json");

    LOG_INFO << "Starting inventory system server on port 7777 with proper controller architecture";

    // Create ProductsController instance for API routing
    auto productsController = std::make_shared<ProductsController>();

    // Map /api/products routes to ProductsController methods
    drogon::app().registerHandler(
        "/api/products",
        [productsController](const drogon::HttpRequestPtr& req,
                             std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
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
    drogon::app().registerHandler(
        "/api/products/{id}",
        [productsController](const drogon::HttpRequestPtr& req,
                             std::function<void(const drogon::HttpResponsePtr&)>&& callback,
                             std::string&& id) {
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
    drogon::app().registerHandler(
        "/health", [](const drogon::HttpRequestPtr& req,
                      std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
            Json::Value response;
            response["status"] = "healthy";
            response["service"] = "inventory-system";
            response["timestamp"] = static_cast<Json::Int64>(std::time(nullptr));
            auto resp = drogon::HttpResponse::newHttpJsonResponse(response);
            callback(resp);
        });

    // API documentation endpoint
    drogon::app().registerHandler(
        "/api", [](const drogon::HttpRequestPtr& req,
                   std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
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
            endpoints.append("GET / - Home page with product list");
            endpoints.append("GET /create - Web form to create products");
            response["endpoints"] = endpoints;
            auto resp = drogon::HttpResponse::newHttpJsonResponse(response);
            callback(resp);
        });

    // Web form for creating products
    drogon::app().registerHandler(
        "/create", [](const drogon::HttpRequestPtr& req,
                      std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
            if (req->getMethod() == drogon::Get) {
                // Serve the HTML form
                auto resp = drogon::HttpResponse::newHttpViewResponse("create_product");
                callback(resp);
            } else {
                // Method not allowed for this endpoint (form posts go to /api/products)
                auto resp = drogon::HttpResponse::newHttpResponse();
                resp->setStatusCode(drogon::k405MethodNotAllowed);
                callback(resp);
            }
        });

    // Home page redirect to create form
    drogon::app().registerHandler(
        "/", [](const drogon::HttpRequestPtr& req,
                std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
            auto resp = drogon::HttpResponse::newRedirectionResponse("/create");
            callback(resp);
        });

    LOG_INFO << "Controller architecture initialized:";
    LOG_INFO << "  - ProductsController: Business logic in controllers/ProductsController.cc";
    LOG_INFO << "  - API Routes: /api/products/* mapped to ProductsController methods";
    LOG_INFO << "  - Clean separation: main.cc handles routing, controllers handle business logic";

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
