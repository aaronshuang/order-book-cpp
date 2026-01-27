#pragma once
#include "Limit.hpp"
#include <unordered_map>
#include <map>


class OrderBook {
private:
    std::map<uint64_t, Limit*, std::greater<uint64_t>> bids;
    std::map<uint64_t, Limit*> asks;

    std::unordered_map<uint64_t, Order*> orderMap;

public:
    void addOrder(Order* order);
    void cancelOrder(uint64_t orderId);
    void match();
};

