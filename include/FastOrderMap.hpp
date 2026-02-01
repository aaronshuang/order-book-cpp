#pragma once
#include <vector>
#include <cstdint>

// Flat hash map specifically for Order pointers
// Uses power of two sizing for bitmask indexing (faster than modulo)
template <size_t Capacity = 1 << 20>
class FastOrderMap {
private:
    struct Entry {
        uint64_t id = 0;
        Order* ptr = nullptr;
    };

    Entry* table;
    const uint64_t mask = Capacity - 1;
    size_t current_size = 0;

public:
    FastOrderMap() {
        // Allocate aligned memory for better cache performance
        table = (Entry*)aligned_alloc(64, sizeof(Entry) * Capacity);
        std::memset(table, 0, sizeof(Entry) * Capacity);
    }

    ~FastOrderMap() { free(table); }

    inline void insert(uint64_t id, Order* order) {
        uint64_t h = id & mask;
        // Linear probing
        while (table[h].ptr != nullptr) {
            if (table[h].id == id) {
                table[h].ptr = order;
                return;
            }
            h = (h + 1) & mask;
        }
        table[h].id = id;
        table[h].ptr = order;
        current_size++;
    }

    inline Order* find(uint64_t id) const {
        uint64_t h = id & mask;
        while (table[h].ptr != nullptr) {
            if (table[h].id == id) return table[h].ptr;
            h = (h + 1) & mask;
        }
        return nullptr;
    }

    inline void erase(uint64_t id) {
        uint64_t h = id & mask;
        while (table[h].ptr != nullptr) {
            if (table[h].id == id) {
                table[h].id = 0;
                table[h].ptr = nullptr;
                current_size--;
                
                // Rehash neighbors to keep linear probe chain intact
                rehash_neighbors(h);
                return;
            }
            h = (h + 1) & mask;
        }
    }

    inline size_t size() const { return current_size; }

private:
    void rehash_neighbors(uint64_t i) {
        uint64_t j = i;
        while (true) {
            j = (j + 1) & mask;
            if (table[j].ptr == nullptr) break;
            
            uint64_t k = table[j].id & mask;
            // Determine if j is outside the range (i, k]
            if ((j > i && (k <= i || k > j)) || (j < i && (k <= i && k > j))) {
                table[i] = table[j];
                table[j].id = 0;
                table[j].ptr = nullptr;
                i = j;
            }
        }
    }
};