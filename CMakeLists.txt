cmake_minimum_required(VERSION 3.10)
project(OrderBookEngine)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

# 1. The original App
add_executable(order_book_app src/main.cpp src/OrderBook.cpp)

# 2. The New Test Suite
add_executable(run_tests src/tests.cpp src/OrderBook.cpp)

# Compilation flags
if(MSVC)
    target_compile_options(order_book_app PRIVATE /W4)
    target_compile_options(run_tests PRIVATE /W4)
else()
    target_compile_options(order_book_app PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(run_tests PRIVATE -Wall -Wextra -Wpedantic)
endif()