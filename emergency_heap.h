#ifndef EMERGENCY_HEAP_H
#define EMERGENCY_HEAP_H

#include <vector>
#include <utility>
#include <cstddef>
#include "entities.h"

// ---------------------------------------------------------------
// Custom binary max-heap stored in a vector.
//   parent of i      -> (i - 1) / 2
//   children of i    -> 2i + 1 and 2i + 2
// The emergency with the highest priority score sits at index 0.
// If two scores are equal, the smaller emergency id wins (stable,
// repeatable output).
// push / pop are O(log n), reading the top is O(1).
// ---------------------------------------------------------------
class EmergencyHeap {
private:
    std::vector<Emergency> data;

    // true if a should be served before b
    static bool higher(const Emergency& a, const Emergency& b) {
        if (a.priorityScore() != b.priorityScore())
            return a.priorityScore() > b.priorityScore();
        return a.getId() < b.getId();
    }

    // move a newly added element up until the heap rule holds again
    void siftUp(std::size_t i) {
        while (i > 0) {
            std::size_t parent = (i - 1) / 2;
            if (higher(data[i], data[parent])) {
                std::swap(data[i], data[parent]);
                i = parent;
            } else {
                break;
            }
        }
    }

    // move the element at the root down after a pop
    void siftDown(std::size_t i) {
        std::size_t n = data.size();
        while (true) {
            std::size_t left = 2 * i + 1;
            std::size_t right = 2 * i + 2;
            std::size_t best = i;

            if (left < n && higher(data[left], data[best])) best = left;
            if (right < n && higher(data[right], data[best])) best = right;

            if (best == i) break;
            std::swap(data[i], data[best]);
            i = best;
        }
    }

public:
    bool empty() const { return data.empty(); }
    std::size_t size() const { return data.size(); }

    void push(const Emergency& e) {
        data.push_back(e);
        siftUp(data.size() - 1);
    }

    // Caller must check empty() first
    Emergency pop() {
        Emergency top = data[0];
        data[0] = data.back();
        data.pop_back();
        if (!data.empty()) siftDown(0);
        return top;
    }
};

#endif
