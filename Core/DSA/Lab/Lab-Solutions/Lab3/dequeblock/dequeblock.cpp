#pragma once
#include "dequeblock.h"
#include "deque.h"

// ============================================================
// PART 1: dequeBlock<T> -- fixed-capacity circular-array deque
// ============================================================

template <typename T>
dequeBlock<T>::dequeBlock(int capacity) {
    // Your code starts from here -- DO NOT EDIT ANYTHING ABOVE

    data = new T[capacity];
    cap = capacity;
    startIdx = 0;
    count = 0;

    // Your code ends here -- DO NOT EDIT ANYTHING BELOW
}

template <typename T>
dequeBlock<T>::~dequeBlock() {
    // Your code starts from here -- DO NOT EDIT ANYTHING ABOVE

    delete[] data;

    // Your code ends here -- DO NOT EDIT ANYTHING BELOW
}

template <typename T>
bool dequeBlock<T>::pushBack(const T& val) {
    // Your code starts from here -- DO NOT EDIT ANYTHING ABOVE

    if (isFull()) return false;
    int idx = (startIdx + count) % cap;
    data[idx] = val;
    count++;
    return true;

    // Your code ends here -- DO NOT EDIT ANYTHING BELOW
}

template <typename T>
bool dequeBlock<T>::pushFront(const T& val) {
    // Your code starts from here -- DO NOT EDIT ANYTHING ABOVE

    if (isFull()) return false;
    // wrap backwards -- this is the only tricky bit compared to pushBack
    startIdx = (startIdx - 1 + cap) % cap;
    data[startIdx] = val;
    count++;
    return true;

    // Your code ends here -- DO NOT EDIT ANYTHING BELOW
}

template <typename T>
bool dequeBlock<T>::popBack(T& outVal) {
    // Your code starts from here -- DO NOT EDIT ANYTHING ABOVE

    if (isEmpty()) return false;
    int idx = (startIdx + count - 1) % cap;
    outVal = data[idx];
    count--;
    return true;

    // Your code ends here -- DO NOT EDIT ANYTHING BELOW
}

template <typename T>
bool dequeBlock<T>::popFront(T& outVal) {
    // Your code starts from here -- DO NOT EDIT ANYTHING ABOVE

    if (isEmpty()) return false;
    outVal = data[startIdx];
    startIdx = (startIdx + 1) % cap;
    count--;
    return true;

    // Your code ends here -- DO NOT EDIT ANYTHING BELOW
}

template <typename T>
T& dequeBlock<T>::front() {
    // Your code starts from here -- DO NOT EDIT ANYTHING ABOVE

    return data[startIdx];

    // Your code ends here -- DO NOT EDIT ANYTHING BELOW
}

template <typename T>
T& dequeBlock<T>::back() {
    // Your code starts from here -- DO NOT EDIT ANYTHING ABOVE

    int idx = (startIdx + count - 1) % cap;
    return data[idx];

    // Your code ends here -- DO NOT EDIT ANYTHING BELOW
}

template <typename T>
T& dequeBlock<T>::operator[](int i) {
    // Your code starts from here -- DO NOT EDIT ANYTHING ABOVE

    // caller guarantees 0 <= i < count, so no bounds check here
    int idx = (startIdx + i) % cap;
    return data[idx];

    // Your code ends here -- DO NOT EDIT ANYTHING BELOW
}

template <typename T>
int dequeBlock<T>::size() const {
    // Your code starts from here -- DO NOT EDIT ANYTHING ABOVE

    return count;

    // Your code ends here -- DO NOT EDIT ANYTHING BELOW
}

template <typename T>
int dequeBlock<T>::capacity() const {
    // Your code starts from here -- DO NOT EDIT ANYTHING ABOVE

    return cap;

    // Your code ends here -- DO NOT EDIT ANYTHING BELOW
}

template <typename T>
bool dequeBlock<T>::isFull() const {
    // Your code starts from here -- DO NOT EDIT ANYTHING ABOVE

    return count == cap;

    // Your code ends here -- DO NOT EDIT ANYTHING BELOW
}

template <typename T>
bool dequeBlock<T>::isEmpty() const {
    // Your code starts from here -- DO NOT EDIT ANYTHING ABOVE

    return count == 0;

    // Your code ends here -- DO NOT EDIT ANYTHING BELOW
}

// ============================================================
// PART 2: Deque<T> -- growing deque built as a directory of
//          fixed-capacity dequeBlocks
// ============================================================

template <typename T>
dequeBlock<T>* Deque<T>::makeBlock() {
    // Your code starts from here -- DO NOT EDIT ANYTHING ABOVE

    return new dequeBlock<T>(blockCapacity);

    // Your code ends here -- DO NOT EDIT ANYTHING BELOW
}

template <typename T>
void Deque<T>::growDirectoryIfNeeded() {
    // Your code starts from here -- DO NOT EDIT ANYTHING ABOVE

    if (!directory->isFull()) return;

    // same doubling trick as std::vector -- just applied to the directory
    int newCap = directory->capacity() * 2;
    dequeBlock<dequeBlock<T>*>* bigger = new dequeBlock<dequeBlock<T>*>(newCap);

    int n = directory->size();
    for (int i = 0; i < n; i++) {
        bigger->pushBack((*directory)[i]);
    }

    delete directory;
    directory = bigger;

    // Your code ends here -- DO NOT EDIT ANYTHING BELOW
}

template <typename T>
Deque<T>::Deque(int blockCapacity) {
    // Your code starts from here -- DO NOT EDIT ANYTHING ABOVE

    directory = new dequeBlock<dequeBlock<T>*>(2);
    this->blockCapacity = blockCapacity;
    totalCount = 0;

    // Your code ends here -- DO NOT EDIT ANYTHING BELOW
}

template <typename T>
Deque<T>::~Deque() {
    // Your code starts from here -- DO NOT EDIT ANYTHING ABOVE

    int n = directory->size();
    for (int i = 0; i < n; i++) {
        delete (*directory)[i];
    }
    delete directory;

    // Your code ends here -- DO NOT EDIT ANYTHING BELOW
}

template <typename T>
void Deque<T>::pushBack(const T& val) {
    // Your code starts from here -- DO NOT EDIT ANYTHING ABOVE

    if (directory->isEmpty() || directory->back()->isFull()) {
        growDirectoryIfNeeded();
        directory->pushBack(makeBlock());
    }
    directory->back()->pushBack(val);
    totalCount++;

    // Your code ends here -- DO NOT EDIT ANYTHING BELOW
}

template <typename T>
void Deque<T>::pushFront(const T& val) {
    // Your code starts from here -- DO NOT EDIT ANYTHING ABOVE

    if (directory->isEmpty() || directory->front()->isFull()) {
        growDirectoryIfNeeded();
        directory->pushFront(makeBlock());
    }
    directory->front()->pushFront(val);
    totalCount++;

    // Your code ends here -- DO NOT EDIT ANYTHING BELOW
}

template <typename T>
bool Deque<T>::popBack(T& outVal) {
    // Your code starts from here -- DO NOT EDIT ANYTHING ABOVE

    if (isEmpty()) return false;

    dequeBlock<T>* lastBlock = directory->back();
    lastBlock->popBack(outVal);
    totalCount--;

    // block emptied out -- drop it from the directory
    if (lastBlock->isEmpty()) {
        dequeBlock<T>* removed;
        directory->popBack(removed);
        delete removed;
    }
    return true;

    // Your code ends here -- DO NOT EDIT ANYTHING BELOW
}

template <typename T>
bool Deque<T>::popFront(T& outVal) {
    // Your code starts from here -- DO NOT EDIT ANYTHING ABOVE

    if (isEmpty()) return false;

    dequeBlock<T>* firstBlock = directory->front();
    firstBlock->popFront(outVal);
    totalCount--;

    if (firstBlock->isEmpty()) {
        dequeBlock<T>* removed;
        directory->popFront(removed);
        delete removed;
    }
    return true;

    // Your code ends here -- DO NOT EDIT ANYTHING BELOW
}

template <typename T>
T& Deque<T>::front() {
    // Your code starts from here -- DO NOT EDIT ANYTHING ABOVE

    return directory->front()->front();

    // Your code ends here -- DO NOT EDIT ANYTHING BELOW
}

template <typename T>
T& Deque<T>::back() {
    // Your code starts from here -- DO NOT EDIT ANYTHING ABOVE

    return directory->back()->back();

    // Your code ends here -- DO NOT EDIT ANYTHING BELOW
}

template <typename T>
T& Deque<T>::operator[](int i) {
    // Your code starts from here -- DO NOT EDIT ANYTHING ABOVE

    int N = blockCapacity;
    int k = directory->front()->size(); // only the front block can be partial

    int d, j;
    if (i < k) {
        d = 0;
        j = i;
    } else {
        d = (i - k) / N + 1;
        j = (i - k) % N;
    }

    return (*(*directory)[d])[j];

    // Your code ends here -- DO NOT EDIT ANYTHING BELOW
}

template <typename T>
int Deque<T>::size() const {
    // Your code starts from here -- DO NOT EDIT ANYTHING ABOVE

    return totalCount;

    // Your code ends here -- DO NOT EDIT ANYTHING BELOW
}

template <typename T>
bool Deque<T>::isEmpty() const {
    // Your code starts from here -- DO NOT EDIT ANYTHING ABOVE

    return totalCount == 0;

    // Your code ends here -- DO NOT EDIT ANYTHING BELOW
}
