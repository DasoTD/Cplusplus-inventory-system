#!/bin/bash

# Navigate to middleware directory
cd middleware

# Create build directory if it doesn't exist
mkdir -p build
cd build

# Configure and build
cmake ..
make

# Check if the main server is running on port 7777
if ! nc -z 127.0.0.1 7777; then
    echo "Error: Test server is not running on port 7777"
    echo "Please start your inventory system server first"
    exit 1
fi

# Run the tests
echo "Running validation middleware tests..."
./validat_middleware_test

echo "Tests completed!"
