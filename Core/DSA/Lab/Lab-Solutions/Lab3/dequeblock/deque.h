#pragma once
#include "common.h"
#include "dequeblock.h"

// Deque<T>: a dynamically-growing deque built as a "directory" of
// fixed-capacity dequeBlocks, following the construction sketched in
// Lecture 08 (Deque, slides on the directory-of-dequeBlocks design).
//
// - The directory itself is a dequeBlock<dequeBlock<T>*> (a dequeBlock
//   whose elements are pointers to data-holding dequeBlocks). When the
//   directory itself becomes full, it is reallocated to double capacity
//   (same amortised-doubling idea used for std::vector / std::deque).
// - Every data block has a fixed capacity of `blockCapacity` elements.
//   At any time, every block EXCEPT possibly the front-most and the
//   rear-most block is completely full -- this invariant is what makes
//   O(1) random access via operator[] possible.
template <typename T>
class Deque {
private:
    dequeBlock<dequeBlock<T>*>* directory; // pointers to data blocks
    int blockCapacity;                      // capacity (N) of each data block
    int totalCount;                          // total number of elements stored

    dequeBlock<T>* makeBlock();
    void growDirectoryIfNeeded();

public:
    explicit Deque(int blockCapacity = 4);
    ~Deque();

    Deque(const Deque&) = delete;
    Deque& operator=(const Deque&) = delete;

    void pushBack(const T& val);
    void pushFront(const T& val);

    bool popBack(T& outVal);   // false if the deque is empty
    bool popFront(T& outVal);  // false if the deque is empty

    T& front();
    T& back();

    T& operator[](int i); // O(1) random access via the (d, j) translation

    int size() const;
    bool isEmpty() const;
};

// Template classes -- implementations must be visible at instantiation sites.
// dequeblock.cpp holds BOTH implementations (Part 1 and Part 2) and is pulled
// in here, after both dequeBlock<T> and Deque<T> have been declared.
#include "dequeblock.cpp"
