-- sample_data.sql
INSERT INTO suppliers (name, contact_info) VALUES
('Supplier A', 'contact@suppliera.com'),
('Supplier B', 'contact@supplierb.com');

INSERT INTO warehouses (name, location, capacity) VALUES
('Warehouse 1', 'New York', 1000),
('Warehouse 2', 'Los Angeles', 800);

INSERT INTO products (sku, name, description, reorder_threshold, quantity_in_stock, supplier_id, warehouse_id) VALUES
('SKU001', 'Laptop', 'High-end laptop', 10, 5, 1, 1),
('SKU002', 'Phone', 'Smartphone', 15, 20, 2, 2);

INSERT INTO purchase_orders (product_id, supplier_id, warehouse_id, quantity_ordered, order_date, expected_arrival_date, status) VALUES
(1, 1, 1, 20, '2025-10-13', '2025-10-16', 'PENDING');

