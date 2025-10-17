#!/bin/bash

# Simple script to build and run the inventory system server

set -e

echo "🔨 Building inventory system..."

# Create build directory if it doesn't exist
mkdir -p build
cd build

# Configure and build
cmake .. && make

echo "✅ Build completed successfully!"

echo "🚀 Starting inventory system server..."

# Run the server (logs to both console and file)
echo "Server starting on http://localhost:7777"
echo "Health check: http://localhost:7777/health"
echo "API docs: http://localhost:7777/api"
echo ""
echo "Press Ctrl+C to stop the server"
echo ""

# Start the server from the root directory where config.json is located
cd ..
./build/inventory_system 2>&1 | tee server.log