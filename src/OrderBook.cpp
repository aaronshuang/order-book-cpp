#include "OrderBook.hpp"
#include <algorithm>
#include <iostream>

OrderBook::OrderBook() : pool(100000) {}

void OrderBook::addOrder(Order* order) {
    // Try to match immediately
    if (order->side == Side::Buy) {
        // While we can match Buys to Asks
        while (order->quantity > 0 && !asks.empty()) {
            Limit* bestAsk = asks.begin()->second;

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
                    // pool.release(restingOrder); 
                    restingOrder = nextOrder;
                } else {
                    restingOrder = restingOrder->next;
                }
            }

            // Cleanup Price Level if empty
            if (bestAsk->orderCount == 0) {
                delete bestAsk;
                asks.erase(asks.begin());
            }
        }
    } 
    else {
        // While we can match Sells to Bids
        while (order->quantity > 0 && !bids.empty()) {
            Limit* bestBid = bids.begin()->second;

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
                    // pool.release(restingOrder);
                    restingOrder = nextOrder;
                } else {
                    restingOrder = restingOrder->next;
                }
            }

            if (bestBid->orderCount == 0) {
                delete bestBid;
                bids.erase(bids.begin());
            }
        }
    }
    
    // Add to book if the order was not fully filled
    if (order->quantity > 0) {
        orderMap[order->orderId] = order;
        
        if (order->side == Side::Buy) {
            if (bids.find(order->price) == bids.end()) {
                bids[order->price] = new Limit(order->price);
            }
            bids[order->price]->addOrder(order);
        } else {
            if (asks.find(order->price) == asks.end()) {
                asks[order->price] = new Limit(order->price);
            }
            asks[order->price]->addOrder(order);
        }
    } else {
        // Order was filled instantly
        // pool.release(order);
    }
}

void OrderBook::cancelOrder(uint64_t orderId) {
    if (orderMap.find(orderId) == orderMap.end()) return;

    Order* order = orderMap[orderId];
    if (order->side == Side::Buy) {
        auto it = bids.find(order->price);
        if (it != bids.end()) {
            it->second->removeOrder(order);
            if (it->second->orderCount == 0) {
                delete it->second;
                bids.erase(it);
            }
        }
    }
    else {
        auto it = asks.find(order->price);
        if (it != asks.end()) {
            it->second->removeOrder(order);
            if (it->second->orderCount == 0) {
                delete it->second;
                asks.erase(it);
            }
        }
    }

    orderMap.erase(orderId);
}