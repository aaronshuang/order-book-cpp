#include <iostream>
#include <cassert>
#include "OrderBook.hpp"
#include <random>

// Helper for cleaner test output
#define RUN_TEST(testFunc) \
    std::cout << "Running " << #testFunc << "... "; \
    testFunc(); \
    std::cout << "PASSED" << std::endl;

void test_fifo_priority() {
    OrderBook ob;
    // Two buys at the same price
    ob.addOrder(ob.createOrder(1, 10000, 10, Side::Buy)); // First
    ob.addOrder(ob.createOrder(2, 10000, 10, Side::Buy)); // Second

    // A sell for 10 should hit order 1, leaving order 2 untouched
    ob.addOrder(ob.createOrder(3, 10000, 10, Side::Sell));

    // Verify: Order 1 should be gone, Order 2 should still exist
    assert(ob.getOrderCount() == 1);
    assert(ob.getVolumeAtPrice(Side::Buy, 10000) == 10);
    
    // Attempting to cancel 1 should do nothing (it's filled), 
    // cancelling 2 should empty the book.
    ob.cancelOrder(2);
    assert(ob.getOrderCount() == 0);
}

void test_spread_boundary() {
    OrderBook ob;
    ob.addOrder(ob.createOrder(1, 10000, 10, Side::Buy));  // Bid $100.00
    ob.addOrder(ob.createOrder(2, 10001, 10, Side::Sell)); // Ask $100.01
    
    // Best Bid < Best Ask: Should NOT match
    assert(ob.getOrderCount() == 2);
    assert(ob.getBestBid() == 10000);
    assert(ob.getBestAsk() == 10001);

    // Incoming Buy at 10001 should clear the Ask
    ob.addOrder(ob.createOrder(3, 10001, 10, Side::Buy));
    assert(ob.getBestAsk() == 0); // No more asks
    assert(ob.getBestBid() == 10000);
}

void test_multiple_level_exhaustion() {
    OrderBook ob;
    // Stack the asks: 10 @ 101, 10 @ 102, 10 @ 103
    ob.addOrder(ob.createOrder(1, 10100, 10, Side::Sell));
    ob.addOrder(ob.createOrder(2, 10200, 10, Side::Sell));
    ob.addOrder(ob.createOrder(3, 10300, 10, Side::Sell));

    // Big Buy for 25 @ 103: Should eat 101, 102, and half of 103
    ob.addOrder(ob.createOrder(4, 10300, 25, Side::Buy));

    assert(ob.getVolumeAtPrice(Side::Sell, 10100) == 0);
    assert(ob.getVolumeAtPrice(Side::Sell, 10200) == 0);
    assert(ob.getVolumeAtPrice(Side::Sell, 10300) == 5);
    assert(ob.getOrderCount() == 1); // Only order 3 remains (partially)
}

void test_cancel_non_existent() {
    OrderBook ob;
    ob.addOrder(ob.createOrder(1, 10000, 10, Side::Buy));
    // Cancel ID that doesn't exist
    ob.cancelOrder(999); 
    // Cancel same ID twice
    ob.cancelOrder(1);
    ob.cancelOrder(1);
    assert(ob.getOrderCount() == 0);
}

void test_full_cycle_memory_recycling() {
    OrderBook ob;
    // Force a high churn rate in the pool
    for (int i = 0; i < 500000; ++i) {
        // Add and immediately fill
        ob.addOrder(ob.createOrder(i, 10000, 10, Side::Buy));
        ob.addOrder(ob.createOrder(i + 1000000, 10000, 10, Side::Sell));
    }
    // If memory isn't recycling, this will throw "Pool Exhausted"
    assert(ob.getOrderCount() == 0);
}

void test_high_volatility_benchmark() {
    std::cout << "\n--- Test: High Volatility FlatMap Benchmark (1M Orders) ---" << std::endl;
    OrderBook ob;
    
    const int NUM_ORDERS = 1000000;
    std::mt19937 rng(1337); 
    
    // We simulate a "Random Walk" for the price to create many Limit levels
    std::uniform_int_distribution<int> price_offset(-5, 5);
    std::uniform_int_distribution<int> side_dist(0, 1);
    uint64_t current_price = 10000;

    auto start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < NUM_ORDERS; ++i) {
        // Price drifts over time, creating a deep book
        current_price += price_offset(rng);
        Side side = (side_dist(rng) == 0) ? Side::Buy : Side::Sell;
        
        // Every 5th order is a "Market Cross" to trigger matching logic
        uint64_t order_price = current_price;
        if (i % 5 == 0) {
            order_price = (side == Side::Buy) ? current_price + 10 : current_price - 10;
        }

        ob.addOrder(ob.createOrder(i, order_price, 10, side));

        // Periodic cancellations to simulate real-world churn
        if (i > 200 && i % 10 == 0) {
            ob.cancelOrder(i - 150);
        }
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diff = end - start;

    std::cout << "Volatility Results:" << std::endl;
    std::cout << "Total Time: " << diff.count() << " s" << std::endl;
    std::cout << "Throughput: " << (int)(NUM_ORDERS / diff.count()) << " orders/sec" << std::endl;
    std::cout << "Avg Latency: " << (diff.count() / NUM_ORDERS) * 1e9 << " ns/order" << std::endl;
}

int main() {
    std::cout << "--- STARTING COMPREHENSIVE VALIDATION ---" << std::endl;
    
    RUN_TEST(test_fifo_priority);
    RUN_TEST(test_spread_boundary);
    RUN_TEST(test_multiple_level_exhaustion);
    RUN_TEST(test_cancel_non_existent);
    RUN_TEST(test_full_cycle_memory_recycling);
    RUN_TEST(test_high_volatility_benchmark);

    std::cout << "--- ALL CORE LOGIC VERIFIED ---" << std::endl;
    return 0;
}