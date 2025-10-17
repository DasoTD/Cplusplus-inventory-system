#!/bin/bash

# Check if the main server is running on port 7777
if ! nc -z 127.0.0.1 7777; then
    echo "Error: Test server is not running on port 7777"
    echo "Please start your inventory system server first"
    exit 1
fi

# Run the tests
echo "Running validation middleware tests..."
./middleware/simple_validation_test

echo "Tests completed!"
