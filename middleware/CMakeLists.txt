cmake_minimum_required(VERSION 3.5)
project(ValidationMiddlewareTest)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(GTest REQUIRED)
find_package(Drogon CONFIG REQUIRED)

# Create test executable
add_executable(validat_middleware_test validatMiddlewareTest.cc)

# Link libraries using the same approach as main project
target_link_libraries(validat_middleware_test 
    PRIVATE
    Drogon::Drogon
    GTest::GTest
    GTest::Main
    pthread
)
