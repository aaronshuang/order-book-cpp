#include "OrderBook.hpp"
#include <algorithm>
#include <iostream>

void OrderBook::addOrder(Order* order) {
    // Try to match immediately
    if (order->side == Side::Buy) {
        // While we can match Buys to Asks
        while (order->quantity > 0 && !asks.empty()) {
            Limit* bestAsk = asks.getBestValue();

            // Check Price Condition
            if (order->price < bestAsk->price) break; 

            Order* restingOrder = bestAsk->head;
            while (order->quantity > 0 && restingOrder) {
                uint32_t fillQty = std::min(order->quantity, restingOrder->quantity);
                
                order->quantity -= fillQty;
                restingOrder->quantity -= fillQty;
                bestAsk->totalVolume -= fillQty;

                // Move to next order if current ask is filled
                if (restingOrder->quantity == 0) {
                    Order* nextOrder = restingOrder->next;
                    bestAsk->removeOrder(restingOrder);
                    orderMap.erase(restingOrder->orderId);
                    orderPool.release(restingOrder); 
                    restingOrder = nextOrder;
                } else {
                    restingOrder = restingOrder->next;
                }
            }

            // Cleanup Price Level if empty
            if (bestAsk->orderCount == 0) {
                // delete bestAsk;
                asks.popBest();
                limitPool.release(bestAsk);
            }
        }
    } 
    else {
        // While we can match Sells to Bids
        while (order->quantity > 0 && !bids.empty()) {
            Limit* bestBid = bids.getBestValue();

            if (order->price > bestBid->price) break;

            Order* restingOrder = bestBid->head;
            while (order->quantity > 0 && restingOrder) {
                uint32_t fillQty = std::min(order->quantity, restingOrder->quantity);
                
                order->quantity -= fillQty;
                restingOrder->quantity -= fillQty;
                bestBid->totalVolume -= fillQty;

                if (restingOrder->quantity == 0) {
                    Order* nextOrder = restingOrder->next;
                    bestBid->removeOrder(restingOrder);
                    orderMap.erase(restingOrder->orderId);
                    orderPool.release(restingOrder);
                    restingOrder = nextOrder;
                } else {
                    restingOrder = restingOrder->next;
                }
            }

            if (bestBid->orderCount == 0) {
                // delete bestBid;
                bids.popBest();
                limitPool.release(bestBid);
            }
        }
    }
    
    // Add to book if the order was not fully filled
    if (order->quantity > 0) {
        // orderMap[order->orderId] = order;
        orderMap.insert(order->orderId, order);
        // Single pass lookup/insert
        if (order->side == Side::Buy) {
            Limit*& limitRef = bids.getOrCreate(order->price);
            if (!limitRef) {
                limitRef = limitPool.acquire(order->price);
            }
            limitRef->addOrder(order);
        }
        else {
            Limit*& limitRef = asks.getOrCreate(order->price);
            if (!limitRef) {
                limitRef = limitPool.acquire(order->price);
            }
            limitRef->addOrder(order);
        }
    } 
    else {
        // Instantly Filled
        orderPool.release(order);
    }
}

void OrderBook::cancelOrder(uint64_t orderId) {
    // if (orderMap.find(orderId) == orderMap.end()) return;

    // Order* order = orderMap[orderId];
    Order* order = orderMap.find(orderId);
    if (!order) return;
    if (order->side == Side::Buy) {
        int idx = bids.indexOf(order->price);
        if (idx != -1) {
            Limit* limit = bids.getOrCreate(order->price);
            limit->removeOrder(order);
            if (limit->orderCount == 0) {
                limitPool.release(limit);
                bids.remove(order->price);
            }
        }
    }
    else {
        int idx = asks.indexOf(order->price);
        if (idx != -1) {
            Limit* limit = asks.getOrCreate(order->price);
            limit->removeOrder(order);
            if (limit->orderCount == 0) {
                limitPool.release(limit);
                asks.remove(order->price);
            }
        }
    }

    orderMap.erase(orderId);
    orderPool.release(order);
}