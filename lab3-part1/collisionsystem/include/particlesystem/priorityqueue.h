#pragma once

#include <iostream>
#include <vector>
#include <cassert>

// #define TEST_PRIORITY_QUEUE

/**
 * A heap based priority queue where the root is the smallest element -- min heap
 */
template <class Comparable>
class PriorityQueue {
public:
    /**
     * Constructor to create a queue with the given capacity
     */
    explicit PriorityQueue(int initCapacity = 100) : orderOK{true} {
        pq.reserve(initCapacity + 1);
        makeEmpty();
        assert(isEmpty());
    }

    /**
     * Constructor to initialize a priority queue based on a given vector V
     * Assumes V[0] is not used
     */
    explicit PriorityQueue(const std::vector<Comparable>& V) : pq{V} {
        heapify();
#ifdef TEST_PRIORITY_QUEUE
        assert(isMinHeap());
#endif
    }

    /**
     * Make the queue empty
     */
    void makeEmpty() {
        pq.clear();
        pq.push_back(Comparable{});
    }

    /**
     * Check is the queue is empty
     * Return true if the queue is empty, false otherwise
     */
    bool isEmpty() const {
        return pq.size() == 1;  // slot zero is not used
    }

    /**
     * Get the size of the queue, i.e. number of elements in the queue
     */
    size_t size() const { return pq.size() - 1; }

    /**
     * Get the smallest element in the queue
     */
    Comparable findMin() {
        assert(isEmpty() == false);
        return pq[1];
    }

    /**
     * Remove and return the smallest element in the queue
     */
    Comparable deleteMin();

    /**
     * Add a new element x to the queue
     */
    void insert(const Comparable& x);

    /**
     * Insert element x in the end of the queue, without preserving the heap property
     */
    void toss(const Comparable& x);

private:
    std::vector<Comparable> pq;  // slot with index 0 not used
    bool orderOK;  // flag to keep internal track of when the heap is ordered / not ordered

    // Auxiliary member functions

    /**
     * Restore the heap-ordering property
     */
    void heapify();

    void percolateDown(size_t i);

    /**
     * Test whether pq is a min heap
     */
    bool isMinHeap() const {
        // If the heap has only one element, return true
        if (std::ssize(pq) == 2) return true;

        // For every element in the heap
        for (int i = 1; i < std::ssize(pq)/2; i++)
        {
            // Check if the children are greater than the parent
            if (pq[i] >= pq[2 * i] || (i < (std::ssize(pq) - 1) / 2 && pq[i] >= pq[2 * i + 1]))
                return false;
        }

        return true;
    }
};

template <class Comparable>
void PriorityQueue<Comparable>::percolateDown(size_t i) {
    Comparable temp = pq[i];
    auto c = 2 * i;  // left child

    while (c < pq.size()) {
        if (c < pq.size() - 1) {
            if (pq[c + 1] < pq[c])  // largest child?
                c++;
        }
        // percolate down
        if (pq[c] < temp) {
            pq[i] = pq[c];
            i = c;
            c = 2 * i;
        } else {
            break;
        }
    }
    pq[i] = temp;
}

/**
 * Restore the heap property
 */
template <class Comparable>
void PriorityQueue<Comparable>::heapify() {
    assert(pq.size() > 1);  // slot zero is not used

    for (size_t i = (pq.size() - 1) / 2; i >= 1; --i) {
        percolateDown(i);
    }
    orderOK = true;
}

/**
 * Remove and return the smallest element in the queue
 */
template <class Comparable>
Comparable PriorityQueue<Comparable>::deleteMin() {
    assert(!isEmpty());

    if (!orderOK) {
        heapify();
    }

    Comparable x = pq[1];
    Comparable y = pq[pq.size() - 1];

    pq[1] = y;  // set last element in the heap as the new root
    percolateDown(1);
    pq.pop_back();

#ifdef TEST_PRIORITY_QUEUE
    assert(isMinHeap());
#endif
    return x;
}

/**
 * Insert element x on the last slot, without preserving the heap property
 */
template <class Comparable>
void PriorityQueue<Comparable>::toss(const Comparable& x) {
    orderOK = false;
    pq.push_back(x);
}

/**
 * Add a new element x to the queue
 */
template <class Comparable>
void PriorityQueue<Comparable>::insert(const Comparable& x) {
    pq.push_back(x);            // Append the value at the end of the heap
    int i = std::ssize(pq) - 1; // Starting position is the last element

    // Percolate up
    while (pq[i / 2] > pq[i])
    {
        std::swap(pq[i / 2], pq[i]);
        i = i / 2;
    }

#ifdef TEST_PRIORITY_QUEUE
    assert(isMinHeap());
#endif
}
