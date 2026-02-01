#pragma once
#include <cstring> // For memmove
#include <algorithm>

template <typename Key, typename Value, typename Compare, size_t MaxLevels = 2048>
class FlatMap {
private:
    Key keys[MaxLevels];
    Value values[MaxLevels];
    size_t count = 0;
    Compare comp;

public:
    // O(1) Access to Best
    inline Value getBestValue() const { return values[count - 1]; }
    inline Key getBestKey() const { return keys[count - 1]; }
    inline void popBest() { --count; }

    inline int indexOf(const Key& key) const {
        // Linear scan for small searches
        if (count < 16) {
            for (size_t i = 0; i < count; ++i) {
                if (!comp(keys[i], key) && !comp(key, keys[i])) return (int)i;
            }
            return -1;
        }

        // Binary Search for larger books
        const Key* it = std::lower_bound(keys, keys + count, key, comp);
        if (it != keys + count && !comp(*it, key) && !comp(key, *it)) {
            return (int)(it - keys);
        }
        return -1;
    }

    Value& getOrCreate(const Key& key) {
        Key* it = std::lower_bound(keys, keys + count, key, comp);
        size_t idx = it - keys;

        if (it != keys + count && !comp(*it, key) && !comp(key, *it)) {
            return values[idx];
        }

        // Shift data using memmove
        if (idx < count) {
            std::memmove(&keys[idx + 1], &keys[idx], (count - idx) * sizeof(Key));
            std::memmove(&values[idx + 1], &values[idx], (count - idx) * sizeof(Value));
        }

        keys[idx] = key;
        values[idx] = nullptr;
        ++count;
        return values[idx];
    }

    inline void remove(const Key& key) {
        int idx = indexOf(key);
        if (idx != -1) {
            // Shift back using memmove
            std::memmove(&keys[idx], &keys[idx + 1], (count - idx - 1) * sizeof(Key));
            std::memmove(&values[idx], &values[idx + 1], (count - idx - 1) * sizeof(Value));
            --count;
        }
    }

    inline bool empty() const { return count == 0; }
    inline size_t size() const { return count; }
};