#include <iostream>
#include <iomanip>
#include "OrderBook.hpp"

// Helper to print the "Tape" (Trade results)
void printOrder(const Order* o) {
    std::cout << "ID: " << o->orderId 
              << " | " << (o->side == Side::Buy ? "BUY " : "SELL") 
              << " | Qty: " << o->quantity 
              << " | Price: " << o->price / 100.0 << std::endl;
}

int main() {
    OrderBook ob;

    std::cout << "--- Initializing Market Simulation ---" << std::endl;

    // 1. Create resting liquidity (The "Book")
    // We use 'new' here because we haven't integrated the MemoryPool yet
    Order* buy1 = new Order(1, 10050, 10, Side::Buy);  // $100.50
    Order* buy2 = new Order(2, 10040, 20, Side::Buy);  // $100.40
    
    ob.addOrder(buy1);
    ob.addOrder(buy2);
    std::cout << "Added 2 Buy orders (Liquidity created)." << std::endl;

    // 2. Scenario: Partial Fill
    // An incoming Sell order for 5 shares at $100.50. 
    // This should match against buy1 and leave 5 shares remaining on buy1.
    std::cout << "\nScenario: Incoming Sell (Partial Fill)..." << std::endl;
    Order* sell1 = new Order(3, 10050, 5, Side::Sell);
    ob.addOrder(sell1); 
    
    std::cout << "After match, buy1 quantity is now: " << buy1->quantity << " (Expected: 5)" << std::endl;

    // 3. Scenario: Walking the Book (Price Crossing)
    // A large Sell order at a lower price should clear buy1 AND start hitting buy2.
    std::cout << "\nScenario: Large Sell (Walking the Book)..." << std::endl;
    Order* sell2 = new Order(4, 10030, 15, Side::Sell);
    ob.addOrder(sell2); 
    
    // sell2 (15) hits remaining buy1 (5) -> sell2 has 10 left.
    // sell2 (10) hits buy2 (20) -> buy2 has 10 left.
    std::cout << "After match, buy2 quantity is now: " << buy2->quantity << " (Expected: 10)" << std::endl;

    // 4. Scenario: Cancellation
    std::cout << "\nScenario: Cancellation..." << std::endl;
    ob.cancelOrder(2); // Cancel buy2
    std::cout << "Order 2 (buy2) cancelled." << std::endl;

    std::cout << "\n--- Simulation Complete ---" << std::endl;

    return 0;
}