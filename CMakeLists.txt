cmake_minimum_required(VERSION 3.10)
project(OrderBookEngine)

# Set C++20 standard - highly preferred by firms like Roblox/Meta
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 1. Tell CMake where your header files are (.hpp)
include_directories(include)

# 2. Collect all your source files (.cpp)
set(SOURCES 
    src/main.cpp 
    src/OrderBook.cpp
)

# 3. Create the executable
add_executable(order_book_app ${SOURCES})

# 4. Optional: Add compiler warnings (prevents "sloppy" code bugs)
if(MSVC)
    target_compile_options(order_book_app PRIVATE /W4)
else()
    target_compile_options(order_book_app PRIVATE -Wall -Wextra -Wpedantic)
endif()