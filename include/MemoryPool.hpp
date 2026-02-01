#pragma once
#include <vector>
#include <stdexcept>
#include "Order.hpp"

template <typename T>
class MemoryPool {
private:
    T* pool;
    std::vector<T*> freeList;

public:
    MemoryPool(size_t size) {
        pool = static_cast<T*>(::operator new(size * sizeof(T)));

        // Fill the free list with pointers to pool slots
        for (size_t i = 0; i < size; i++) {
            freeList.push_back(&pool[i]);
        }
    }

    template <typename... Args>
    T* acquire(Args&&... args) {
        if (freeList.empty()) {
            throw std::runtime_error("Pool Exhausted");
        }
        T* object = freeList.back();
        freeList.pop_back();

        new (object) T(std::forward<Args>(args)...);

        return object;
    }

    void release(T* object) {
        freeList.push_back(object);
    }

    ~MemoryPool(){
        ::operator delete(pool);
    }
};