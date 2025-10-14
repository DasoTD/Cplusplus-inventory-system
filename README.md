# C++ Inventory Management System

[![Build Status](https://github.com/DasoTD/Cplusplus-inventory-system/workflows/CI%2FCD%20Pipeline/badge.svg)](https://github.com/DasoTD/Cplusplus-inventory-system/actions)
[![Build and Test](https://github.com/DasoTD/Cplusplus-inventory-system/workflows/Build%20and%20Test/badge.svg)](https://github.com/DasoTD/Cplusplus-inventory-system/actions)

A modern, high-performance inventory management system built with C++ and the Drogon web framework.

## 🚀 Features

- **RESTful API**: Clean REST endpoints for inventory management
- **Modern C++**: Built with C++17 standards and best practices
- **High Performance**: Asynchronous I/O with Drogon framework
- **Database Integration**: SQLite with Drogon ORM
- **Clean Architecture**: Proper separation of concerns with controllers and models
- **CORS Support**: Ready for frontend integration
- **JSON API**: Full JSON API for easy integration

## 📋 API Endpoints

### REST API
| Method | Endpoint | Description |
|--------|----------|-------------|
| GET | `/health` | Health check |
| GET | `/api` | API documentation |
| GET | `/api/products` | List all products |
| GET | `/api/products/{id}` | Get product by ID |
| POST | `/api/products` | Create new product |
| PUT | `/api/products/{id}` | Update product |
| DELETE | `/api/products/{id}` | Delete product |

### Web Interface
| Method | Endpoint | Description |
|--------|----------|-------------|
| GET | `/` | Home page (redirects to create form) |
| GET | `/create` | Web form for creating products |

## 🛠️ Tech Stack

- **Language**: C++17
- **Web Framework**: [Drogon](https://github.com/drogonframework/drogon)
- **Database**: SQLite3 with Drogon ORM
- **Build System**: CMake
- **JSON Library**: JsonCpp
- **Testing**: Custom integration tests

## 🏗️ Building from Source

### Prerequisites

- C++17 compatible compiler (GCC 8+ or Clang 6+)
- CMake 3.15+
- Git
- SQLite3 development libraries

### Ubuntu/Debian
```bash
sudo apt-get update
sudo apt-get install -y build-essential cmake git libssl-dev zlib1g-dev \
    libjsoncpp-dev uuid-dev sqlite3 libsqlite3-dev libbrotli-dev ninja-build
```

### macOS
```bash
brew install cmake ninja jsoncpp sqlite3 openssl@3 ossp-uuid
```

### Install Drogon Framework
```bash
git clone https://github.com/drogonframework/drogon.git
cd drogon
git checkout v1.9.1
git submodule update --init
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -G Ninja
ninja -j$(nproc)
sudo ninja install
sudo ldconfig  # Linux only
```

### Build the Project
```bash
git clone https://github.com/DasoTD/Cplusplus-inventory-system.git
cd Cplusplus-inventory-system
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -G Ninja
ninja -j$(nproc)
```

## 🚀 Running the Application

1. **Start the server**:
   ```bash
   ./build/inventory_system
   ```

2. **Test the API**:
   ```bash
   # Health check
   curl http://localhost:7777/health
   
   # Get all products
   curl http://localhost:7777/api/products
   
   # Create a new product
   curl -X POST http://localhost:7777/api/products \
     -H "Content-Type: application/json" \
     -d '{"sku":"TEST001","name":"Test Product","description":"Test","quantity_in_stock":100,"reorder_threshold":10,"supplier_id":1,"warehouse_id":1}'
   ```

## 🧪 Testing

The project includes automated testing in GitHub Actions:

- **Build Tests**: Multi-platform builds (Linux, macOS, Windows)
- **Integration Tests**: API endpoint testing
- **Static Analysis**: Code quality checks with cppcheck and clang-tidy
- **Security Scanning**: Vulnerability scanning with Trivy

Run tests locally:
```bash
cd build

# Build and run tests
ninja

# Run unit tests (if available)
./test/inventory_system_test

# Run integration tests
./inventory_system &
SERVER_PID=$!
sleep 3

# Test API endpoints
curl -f http://localhost:7777/health
curl -f http://localhost:7777/api
echo "Testing product creation..."
curl -X POST http://localhost:7777/api/products \
  -H "Content-Type: application/json" \
  -d '{"sku":"TEST001","name":"Test Product","quantity_in_stock":10,"reorder_threshold":5,"supplier_id":1,"warehouse_id":1}'

# Cleanup
kill $SERVER_PID
```

## 📁 Project Structure

```
├── .github/workflows/     # GitHub Actions CI/CD
├── controllers/           # REST API controllers
│   ├── ProductsController.h/.cc
│   ├── SuppliersController.h/.cc
│   ├── WarehousesController.h/.cc
│   └── PurchaseOrdersController.h/.cc
├── models/               # Auto-generated ORM models
│   ├── Products.h/.cc
│   ├── Suppliers.h/.cc
│   ├── Warehouses.h/.cc
│   ├── PurchaseOrders.h/.cc
│   └── model.json       # ORM configuration
├── build/               # Build directory
├── CMakeLists.txt       # Build configuration
├── main.cc             # Application entry point
├── config.json         # Database configuration
└── inventory.db        # SQLite database
```

## 🔧 Configuration

The application can be configured via `config.json`:

```json
{
    "listeners": [
        {
            "address": "0.0.0.0",
            "port": 7777,
            "https": false
        }
    ],
    "db_clients": [
        {
            "name": "default",
            "rdbms": "sqlite3",
            "filename": "inventory.db",
            "is_fast": false
        }
    ]
}
```

## 🤝 Contributing

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/amazing-feature`)
3. Make your changes
4. Run tests (`make test`)
5. Format code (`clang-format -i *.cc *.h`)
6. Commit changes (`git commit -m 'Add amazing feature'`)
7. Push to branch (`git push origin feature/amazing-feature`)
8. Open a Pull Request

## 📝 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## 🏆 Acknowledgments

- [Drogon Framework](https://github.com/drogonframework/drogon) - High-performance C++ web framework
- [JsonCpp](https://github.com/open-source-parsers/jsoncpp) - JSON library for C++
- [SQLite](https://www.sqlite.org/) - Embedded database engine

## � Docker Support

### Quick Start with Docker
```bash
# Build the Docker image
docker build -t inventory-system .

# Run the container
docker run -d -p 7777:7777 --name inventory-system inventory-system

# Test the API
curl http://localhost:7777/health
curl http://localhost:7777/create  # Web form for creating products
```

### Docker Compose (Optional)
```bash
# Start services
docker-compose up -d

# View logs
docker-compose logs -f

# Stop services
docker-compose down
```

## 🌐 Web Interface

The application includes a web-based form for creating products:

- **Web Form**: http://localhost:7777/create
- **Home Page**: http://localhost:7777/ (redirects to create form)
- **API Docs**: http://localhost:7777/api

## �📊 Status

- ✅ Core API functionality
- ✅ Database integration with ORM
- ✅ Clean controller architecture
- ✅ Automated CI/CD pipeline
- ✅ Multi-platform builds
- ✅ Integration testing
- ✅ Docker support with multi-stage builds
- ✅ Web interface for product creation
- ✅ Database initialization with sample data
- 🔄 Unit testing (in progress)
- 🔄 Authentication (planned)
