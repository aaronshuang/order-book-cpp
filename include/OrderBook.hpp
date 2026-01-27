#pragma once
#include "Limit.hpp"
#include "MemoryPool.hpp"
#include <unordered_map>
#include <map>


class OrderBook {
private:
    std::map<uint64_t, Limit*, std::greater<uint64_t>> bids;
    std::map<uint64_t, Limit*> asks;

    std::unordered_map<uint64_t, Order*> orderMap;

    OrderPool pool;

public:
    OrderBook() : pool(1200000) {};

    Order* createOrder(uint64_t id, uint64_t price, uint32_t quantity, Side side) {
        return pool.acquire(id, price, quantity, side);
    }

    void addOrder(Order* order);
    void cancelOrder(uint64_t orderId);


    // Testing Getters
    size_t getOrderCount() const {return orderMap.size();}

    uint32_t getVolumeAtPrice(Side side, uint64_t price) {
        if (side == Side::Buy) {
            return (bids.count(price)) ? bids.at(price)->totalVolume : 0;
        }
        else {
            return (asks.count(price)) ? asks.at(price)->totalVolume : 0;
        }
    }

    uint64_t getBestBid() const { return bids.empty() ? 0 : bids.begin()->first; }
    uint64_t getBestAsk() const { return asks.empty() ? 0 : asks.begin()->first; }
};

