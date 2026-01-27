#pragma once
#include "Order.hpp"

struct Limit {
    uint64_t price;
    uint32_t totalVolume;
    uint32_t orderCount;

    Order* head = nullptr;
    Order* tail = nullptr;

    Limit(uint64_t price) : price(price), totalVolume(0), orderCount(0) {}

    void addOrder(Order* order) {
        if (!head) {
            head = order;
            tail = order;
        }
        else {
            tail->next = order;
            order->prev = tail;
            tail = order;
        }
        totalVolume += order->quantity;
        orderCount++;
    }

    void removeOrder(Order* order) {
        if (order->prev) order->prev->next = order->next;
        if (order->next) order->next->prev = order->prev;

        if (order == head) head = order->next;
        if (order == tail) tail = order->prev;

        totalVolume -= order->quantity;
        orderCount--;
    }
};