-- Suppliers Table
CREATE TABLE suppliers (
    supplier_id INTEGER PRIMARY KEY AUTOINCREMENT,
    name TEXT NOT NULL,
    contact_info TEXT NOT NULL
);

-- Warehouses Table
CREATE TABLE warehouses (
    warehouse_id INTEGER PRIMARY KEY AUTOINCREMENT,
    name TEXT NOT NULL,
    location TEXT NOT NULL,
    capacity INTEGER NOT NULL CHECK (capacity >= 0)
);

-- Products Table
CREATE TABLE products (
    product_id INTEGER PRIMARY KEY AUTOINCREMENT,
    sku TEXT NOT NULL UNIQUE,
    name TEXT NOT NULL,
    description TEXT,
    reorder_threshold INTEGER NOT NULL CHECK (reorder_threshold >= 0),
    quantity_in_stock INTEGER NOT NULL CHECK (quantity_in_stock >= 0),
    supplier_id INTEGER,
    warehouse_id INTEGER,
    FOREIGN KEY (supplier_id) REFERENCES suppliers(supplier_id),
    FOREIGN KEY (warehouse_id) REFERENCES warehouses(warehouse_id)
);

-- Purchase Orders Table
CREATE TABLE purchase_orders (
    order_id INTEGER PRIMARY KEY AUTOINCREMENT,
    product_id INTEGER NOT NULL,
    supplier_id INTEGER NOT NULL,
    warehouse_id INTEGER NOT NULL,
    quantity_ordered INTEGER NOT NULL CHECK (quantity_ordered > 0),
    order_date TEXT NOT NULL,
    expected_arrival_date TEXT NOT NULL,
    status TEXT NOT NULL DEFAULT 'PENDING', -- PENDING, COMPLETED
    FOREIGN KEY (product_id) REFERENCES products(product_id),
    FOREIGN KEY (supplier_id) REFERENCES suppliers(supplier_id),
    FOREIGN KEY (warehouse_id) REFERENCES warehouses(warehouse_id)
);
