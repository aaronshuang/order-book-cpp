#pragma once
#include <vector>
#include <stdexcept>
#include "Order.hpp"

class OrderPool {
private:
    Order* pool; // linked list of orders
    std::vector<Order*> freeList;
    size_t poolSize;

public:
    OrderPool(size_t size) : poolSize(size) {
        pool = static_cast<Order*>(::operator new(size * sizeof(Order)));

        for (size_t i = 0; i < size; i++) {
            freeList.push_back(&pool[i]); // fill the free list with pointers to pool slots
        }
    }

    Order* acquire(uint64_t id, uint64_t p, uint32_t q, Side s) {
        if (freeList.empty()) {
            throw std::runtime_error("Pool Exhausted");
        }
        Order* order = freeList.back();
        freeList.pop_back();

        return new (order) Order(id, p, q, s);
    }

    void release(Order* order) {
        freeList.push_back(order);
    }

    ~OrderPool(){
        ::operator delete(pool);
    }
};