#pragma once
#include "common.h"

// dequeBlock<T>: a FIXED-capacity deque implemented using a circular array.
//
// This is the low-level building block used by Deque<T> (Part 2 of this
// lab) to construct a full, dynamically-growing deque, following the
// "directory of dequeBlocks" construction from Lecture 08.
//
// - push/pop from EITHER end run in O(1) time.
// - Random access via operator[] runs in O(1) time.
// - Once the block is full, push operations FAIL (return false) instead of
//   growing -- growth is handled one layer up, by Deque<T>.
template <typename T>
class dequeBlock {
private:
    T* data;
    int cap;
    int startIdx; // circular index of the current front element
    int count;    // number of elements currently stored

public:
    explicit dequeBlock(int capacity);
    ~dequeBlock();

    // A dequeBlock owns raw heap memory -- keep the lab simple by
    // disallowing copies.
    dequeBlock(const dequeBlock&) = delete;
    dequeBlock& operator=(const dequeBlock&) = delete;

    bool pushBack(const T& val);   // false if the block is full
    bool pushFront(const T& val);  // false if the block is full

    bool popBack(T& outVal);   // false if the block is empty
    bool popFront(T& outVal);  // false if the block is empty

    T& front();
    T& back();

    T& operator[](int i); // 0-indexed from the front; caller ensures validity

    int size() const;
    int capacity() const;
    bool isFull() const;
    bool isEmpty() const;
};

// Template class -- the implementation must be visible wherever dequeBlock
// is instantiated. Both dequeBlock<T> and Deque<T> are implemented in the
// single file dequeblock.cpp, which is pulled in at the bottom of deque.h
// (it needs the Deque<T> declaration to be visible first).
#include "deque.h"
