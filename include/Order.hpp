#pragma once
#include <cstdint>

enum class Side : uint8_t {
    Buy = 0,
    Sell = 1
};

struct Order {
    uint64_t orderId;
    uint64_t price;
    uint32_t quantity;
    Side side;

    Order* next = nullptr;
    Order* prev = nullptr;

    Order(uint64_t orderId, uint64_t price, uint32_t quantity, Side side) 
    : orderId(orderId), price(price), quantity(quantity), side(side) {}
};