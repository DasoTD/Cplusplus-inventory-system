# Inventory Management System API Documentation

## Overview

The Inventory Management System is a RESTful API built with C++ and the Drogon framework. It provides comprehensive inventory management capabilities including product management, supplier tracking, warehouse management, and purchase order processing.

**Base URL:** `http://localhost:7777`  
**Version:** 1.0.0  
**Framework:** Drogon (C++)  
**Database:** SQLite3  

## Quick Start

### Start the Server
```bash
./run_server.sh
```

### Health Check
```bash
curl http://localhost:7777/health
```

### API Overview
```bash
curl http://localhost:7777/api
```

## Authentication

Currently, the API does not require authentication. All endpoints are publicly accessible.

## Content Type

All API endpoints expect and return JSON data with `Content-Type: application/json`.

## Error Handling

The API uses standard HTTP status codes and returns consistent error responses:

```json
{
  "error": true,
  "message": "Error description here"
}
```

### Common HTTP Status Codes
- `200 OK` - Successful GET requests
- `201 Created` - Successful POST requests  
- `204 No Content` - Successful PUT/DELETE requests
- `400 Bad Request` - Invalid request data or missing required fields
- `404 Not Found` - Resource not found
- `405 Method Not Allowed` - HTTP method not supported for endpoint
- `500 Internal Server Error` - Server-side error

## Core Endpoints

### Health Check

#### GET /health
Check server health and status.

**Response:**
```json
{
  "status": "healthy",
  "service": "inventory-system", 
  "timestamp": 1729123456
}
```

### API Information

#### GET /api
Get API version and available endpoints.

**Response:**
```json
{
  "service": "Inventory Management System",
  "version": "1.0.0",
  "endpoints": [
    "GET /api/products - List all products",
    "POST /api/products - Create new product", 
    "GET /api/products/{id} - Get product by ID",
    "PUT /api/products/{id} - Update product",
    "DELETE /api/products/{id} - Delete product",
    "GET /health - Health check",
    "GET / - Home page with product list",
    "GET /create - Web form to create products"
  ]
}
```

## Products API

### List All Products

#### GET /api/products
Retrieve all products in the inventory.

**Response:**
```json
[
  {
    "product_id": 1,
    "sku": "WIDGET-001",
    "name": "Super Widget", 
    "description": "High-quality widget for industrial use",
    "unit_price": 29.99,
    "quantity_in_stock": 150,
    "reorder_threshold": 25,
    "supplier_id": 1,
    "warehouse_id": 1
  }
]
```

### Get Product by ID

#### GET /api/products/{id}
Retrieve a specific product by its ID.

**Parameters:**
- `id` (path parameter) - Product ID

**Response:**
```json
{
  "product_id": 1,
  "sku": "WIDGET-001", 
  "name": "Super Widget",
  "description": "High-quality widget for industrial use",
  "unit_price": 29.99,
  "quantity_in_stock": 150,
  "reorder_threshold": 25,
  "supplier_id": 1,
  "warehouse_id": 1
}
```

**Error Response (404):**
```json
{
  "error": true,
  "message": "Product not found"
}
```

### Create New Product

#### POST /api/products
Create a new product in the inventory.

**Request Body:**
```json
{
  "sku": "WIDGET-002",
  "name": "Premium Widget",
  "description": "Premium quality widget", 
  "unit_price": 49.99,
  "quantity_in_stock": 100,
  "reorder_threshold": 20,
  "supplier_id": 1,
  "warehouse_id": 1
}
```

**Required Fields:**
- `sku` (string) - Unique product identifier
- `name` (string) - Product name
- `unit_price` (number) - Price per unit (must be positive)
- `quantity_in_stock` (integer) - Current stock quantity 
- `reorder_threshold` (integer) - Minimum stock level before reordering

**Optional Fields:**
- `description` (string) - Product description
- `supplier_id` (integer) - Associated supplier ID
- `warehouse_id` (integer) - Associated warehouse ID

**Success Response (201):**
```json
{
  "product_id": 2,
  "sku": "WIDGET-002",
  "name": "Premium Widget",
  "description": "Premium quality widget",
  "unit_price": 49.99,
  "quantity_in_stock": 100, 
  "reorder_threshold": 20,
  "supplier_id": 1,
  "warehouse_id": 1
}
```

**Validation Errors (400):**
```json
{
  "error": true,
  "message": "Missing required field: sku"
}
```

```json
{
  "error": true,
  "message": "Unit price must be a positive number"
}
```

### Update Product

#### PUT /api/products/{id}
Update an existing product.

**Parameters:**
- `id` (path parameter) - Product ID

**Request Body:**
```json
{
  "name": "Updated Widget Name",
  "unit_price": 59.99,
  "quantity_in_stock": 75
}
```

**Response (204):** No content on successful update

### Delete Product

#### DELETE /api/products/{id}
Delete a product from the inventory.

**Parameters:**
- `id` (path parameter) - Product ID

**Response (204):** No content on successful deletion

## Web Interface

### Home Page

#### GET /
Redirects to the product creation form.

### Product Creation Form

#### GET /create
Serves an HTML form for creating new products through a web interface.

## Data Models

### Product Schema
```sql
CREATE TABLE products (
    product_id INTEGER PRIMARY KEY AUTOINCREMENT,
    sku TEXT NOT NULL UNIQUE,
    name TEXT NOT NULL,
    description TEXT,
    unit_price REAL NOT NULL,
    reorder_threshold INTEGER NOT NULL CHECK (reorder_threshold >= 0),
    quantity_in_stock INTEGER NOT NULL CHECK (quantity_in_stock >= 0),
    supplier_id INTEGER,
    warehouse_id INTEGER,
    FOREIGN KEY (supplier_id) REFERENCES suppliers(supplier_id),
    FOREIGN KEY (warehouse_id) REFERENCES warehouses(warehouse_id)
);
```

### Supplier Schema
```sql
CREATE TABLE suppliers (
    supplier_id INTEGER PRIMARY KEY AUTOINCREMENT,
    name TEXT NOT NULL,
    contact_info TEXT NOT NULL
);
```

### Warehouse Schema
```sql
CREATE TABLE warehouses (
    warehouse_id INTEGER PRIMARY KEY AUTOINCREMENT,
    name TEXT NOT NULL,
    location TEXT NOT NULL,
    capacity INTEGER NOT NULL CHECK (capacity >= 0)
);
```

### Purchase Order Schema  
```sql
CREATE TABLE purchase_orders (
    order_id INTEGER PRIMARY KEY AUTOINCREMENT,
    product_id INTEGER NOT NULL,
    supplier_id INTEGER NOT NULL,
    warehouse_id INTEGER NOT NULL,
    quantity_ordered INTEGER NOT NULL CHECK (quantity_ordered > 0),
    order_date TEXT NOT NULL,
    expected_arrival_date TEXT NOT NULL,
    status TEXT NOT NULL DEFAULT 'PENDING',
    FOREIGN KEY (product_id) REFERENCES products(product_id),
    FOREIGN KEY (supplier_id) REFERENCES suppliers(supplier_id),
    FOREIGN KEY (warehouse_id) REFERENCES warehouses(warehouse_id)
);
```

## Request Validation

The API includes comprehensive validation middleware that:

- Validates JSON format for all POST/PUT requests
- Ensures required fields are present
- Validates data types and constraints
- Checks for positive unit prices
- Returns descriptive error messages

## CORS Support

The API includes CORS headers for cross-origin requests:
- `Access-Control-Allow-Origin: *`
- `Access-Control-Allow-Methods: GET,POST,PUT,DELETE,OPTIONS` 
- `Access-Control-Allow-Headers: Content-Type`

## Example Usage

### Creating a Product
```bash
curl -X POST http://localhost:7777/api/products \
  -H "Content-Type: application/json" \
  -d '{
    "sku": "LAPTOP-001",
    "name": "Business Laptop", 
    "description": "High-performance laptop for business use",
    "unit_price": 899.99,
    "quantity_in_stock": 50,
    "reorder_threshold": 10,
    "supplier_id": 1,
    "warehouse_id": 1
  }'
```

### Updating a Product
```bash
curl -X PUT http://localhost:7777/api/products/1 \
  -H "Content-Type: application/json" \
  -d '{
    "unit_price": 949.99,
    "quantity_in_stock": 45
  }'
```

### Getting All Products
```bash
curl http://localhost:7777/api/products
```

### Deleting a Product
```bash
curl -X DELETE http://localhost:7777/api/products/1
```

## Testing

### Automated Tests
Run the validation test suite:
```bash
./run_validation_tests.sh
```

The test suite includes:
- Valid product creation tests
- Missing field validation tests  
- Invalid price validation tests
- JSON format validation tests
- Empty request body tests

### Manual Testing
You can test endpoints manually using curl, Postman, or any HTTP client.

## Development

### Starting the Server
```bash
# Build and run
./run_server.sh

# Or manually
mkdir -p build && cd build
cmake .. && make
./inventory_system
```

### Project Structure
```
inventory_system/
├── controllers/          # API controllers
├── models/              # Data models  
├── db/                  # Database initialization
├── validation.cc/.h     # Request validation
├── middleware/          # Middleware components
├── views/               # Web templates
├── config.json          # Configuration
└── schema.sql           # Database schema
```

## Configuration

The server configuration is stored in `config.json`:
```json
{
  "listeners": [
    {
      "address": "0.0.0.0",
      "port": 7777
    }
  ],
  "db_clients": [
    {
      "name": "default",
      "rdbms": "sqlite3", 
      "filename": "inventory.db"
    }
  ]
}
```

## Support

For issues or questions:
1. Check the server logs: `tail -f server.log`
2. Verify server health: `curl http://localhost:7777/health` 
3. Review API status: `curl http://localhost:7777/api`

---

*Generated for Inventory Management System v1.0.0*