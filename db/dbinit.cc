#include "dbinit.h"
#include <drogon/drogon.h>
#include <stdexcept>

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
        throw; // Re-throw to let caller handle the error
    }
}