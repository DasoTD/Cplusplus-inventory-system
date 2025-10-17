#pragma once

/**
 * @brief Initialize the database with required tables and sample data
 * 
 * This function creates all necessary database tables (products, suppliers, 
 * warehouses, purchase_orders) and inserts sample data if tables are empty.
 * 
 * @throws std::exception if database initialization fails
 */
void initializeDatabase();