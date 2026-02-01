#pragma once
#include "Limit.hpp"
#include "MemoryPool.hpp"
#include "FlatMap.hpp"
#include "FastOrderMap.hpp"
#include <unordered_map>
#include <map>


class OrderBook {
private:
    FlatMap<uint64_t, Limit*, std::less<uint64_t>> bids;
    FlatMap<uint64_t, Limit*, std::greater<uint64_t>> asks;

    FastOrderMap<1 << 21> orderMap;

    MemoryPool<Order> orderPool;
    MemoryPool<Limit> limitPool;

public:
    OrderBook() : orderPool(1200000), limitPool(1200000) {};

    Order* createOrder(uint64_t id, uint64_t price, uint32_t quantity, Side side) {
        return orderPool.acquire(id, price, quantity, side);
    }
    Limit* createLimit(uint64_t price) {
        return limitPool.acquire(price);
    }

    void addOrder(Order* order);
    void cancelOrder(uint64_t orderId);


    // Testing Getters
    size_t getOrderCount() const {return orderMap.size();}

    uint32_t getVolumeAtPrice(Side side, uint64_t price) {
        if (side == Side::Buy) {
            int idx = bids.indexOf(price);
            return (idx != -1) ? bids.getOrCreate(price)->totalVolume : 0; 
        } else {
            int idx = asks.indexOf(price);
            return (idx != -1) ? asks.getOrCreate(price)->totalVolume : 0;
        }
    }

    uint64_t getBestBid() const { return bids.empty() ? 0 : bids.getBestKey(); }
    uint64_t getBestAsk() const { return asks.empty() ? 0 : asks.getBestKey(); }
};

