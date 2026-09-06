#pragma once
#include "dequeblock.h"
#include "deque.h"

// ============================================================
// PART 1: dequeBlock<T> -- fixed-capacity circular-array deque
// ============================================================

template <typename T>
dequeBlock<T>::dequeBlock(int capacity) {
    // Your code starts from here -- DO NOT EDIT ANYTHING ABOVE


    // Your code ends here -- DO NOT EDIT ANYTHING BELOW
}

template <typename T>
dequeBlock<T>::~dequeBlock() {
    // Your code starts from here -- DO NOT EDIT ANYTHING ABOVE


    // Your code ends here -- DO NOT EDIT ANYTHING BELOW
}

template <typename T>
bool dequeBlock<T>::pushBack(const T& val) {
    // Your code starts from here -- DO NOT EDIT ANYTHING ABOVE



    // Your code ends here -- DO NOT EDIT ANYTHING BELOW
}

template <typename T>
bool dequeBlock<T>::pushFront(const T& val) {
    // Your code starts from here -- DO NOT EDIT ANYTHING ABOVE



    // Your code ends here -- DO NOT EDIT ANYTHING BELOW
}

template <typename T>
bool dequeBlock<T>::popBack(T& outVal) {
    // Your code starts from here -- DO NOT EDIT ANYTHING ABOVE



    // Your code ends here -- DO NOT EDIT ANYTHING BELOW
}

template <typename T>
bool dequeBlock<T>::popFront(T& outVal) {
    // Your code starts from here -- DO NOT EDIT ANYTHING ABOVE



    // Your code ends here -- DO NOT EDIT ANYTHING BELOW
}

template <typename T>
T& dequeBlock<T>::front() {
    // Your code starts from here -- DO NOT EDIT ANYTHING ABOVE


    // Your code ends here -- DO NOT EDIT ANYTHING BELOW
}

template <typename T>
T& dequeBlock<T>::back() {
    // Your code starts from here -- DO NOT EDIT ANYTHING ABOVE



    // Your code ends here -- DO NOT EDIT ANYTHING BELOW
}

template <typename T>
T& dequeBlock<T>::operator[](int i) {
    // Your code starts from here -- DO NOT EDIT ANYTHING ABOVE



    // Your code ends here -- DO NOT EDIT ANYTHING BELOW
}

template <typename T>
int dequeBlock<T>::size() const {
    // Your code starts from here -- DO NOT EDIT ANYTHING ABOVE


    // Your code ends here -- DO NOT EDIT ANYTHING BELOW
}

template <typename T>
int dequeBlock<T>::capacity() const {
    // Your code starts from here -- DO NOT EDIT ANYTHING ABOVE


    // Your code ends here -- DO NOT EDIT ANYTHING BELOW
}

template <typename T>
bool dequeBlock<T>::isFull() const {
    // Your code starts from here -- DO NOT EDIT ANYTHING ABOVE


    // Your code ends here -- DO NOT EDIT ANYTHING BELOW
}

template <typename T>
bool dequeBlock<T>::isEmpty() const {
    // Your code starts from here -- DO NOT EDIT ANYTHING ABOVE


    // Your code ends here -- DO NOT EDIT ANYTHING BELOW
}

// ============================================================
// PART 2: Deque<T> -- growing deque built as a directory of
//          fixed-capacity dequeBlocks
// ============================================================

template <typename T>
dequeBlock<T>* Deque<T>::makeBlock() {
    // Your code starts from here -- DO NOT EDIT ANYTHING ABOVE


    // Your code ends here -- DO NOT EDIT ANYTHING BELOW
}

template <typename T>
void Deque<T>::growDirectoryIfNeeded() {
    // Your code starts from here -- DO NOT EDIT ANYTHING ABOVE


    // Your code ends here -- DO NOT EDIT ANYTHING BELOW
}

template <typename T>
Deque<T>::Deque(int blockCapacity) {
    // Your code starts from here -- DO NOT EDIT ANYTHING ABOVE


    // Your code ends here -- DO NOT EDIT ANYTHING BELOW
}

template <typename T>
Deque<T>::~Deque() {
    // Your code starts from here -- DO NOT EDIT ANYTHING ABOVE



    // Your code ends here -- DO NOT EDIT ANYTHING BELOW
}

template <typename T>
void Deque<T>::pushBack(const T& val) {
    // Your code starts from here -- DO NOT EDIT ANYTHING ABOVE



    // Your code ends here -- DO NOT EDIT ANYTHING BELOW
}

template <typename T>
void Deque<T>::pushFront(const T& val) {
    // Your code starts from here -- DO NOT EDIT ANYTHING ABOVE



    // Your code ends here -- DO NOT EDIT ANYTHING BELOW
}

template <typename T>
bool Deque<T>::popBack(T& outVal) {
    // Your code starts from here -- DO NOT EDIT ANYTHING ABOVE



    // Your code ends here -- DO NOT EDIT ANYTHING BELOW
}

template <typename T>
bool Deque<T>::popFront(T& outVal) {
    // Your code starts from here -- DO NOT EDIT ANYTHING ABOVE



    // Your code ends here -- DO NOT EDIT ANYTHING BELOW
}

template <typename T>
T& Deque<T>::front() {
    // Your code starts from here -- DO NOT EDIT ANYTHING ABOVE


    // Your code ends here -- DO NOT EDIT ANYTHING BELOW
}

template <typename T>
T& Deque<T>::back() {
    // Your code starts from here -- DO NOT EDIT ANYTHING ABOVE


    // Your code ends here -- DO NOT EDIT ANYTHING BELOW
}

template <typename T>
T& Deque<T>::operator[](int i) {
    // Your code starts from here -- DO NOT EDIT ANYTHING ABOVE



    // Your code ends here -- DO NOT EDIT ANYTHING BELOW
}

template <typename T>
int Deque<T>::size() const {
    // Your code starts from here -- DO NOT EDIT ANYTHING ABOVE


    // Your code ends here -- DO NOT EDIT ANYTHING BELOW
}

template <typename T>
bool Deque<T>::isEmpty() const {
    // Your code starts from here -- DO NOT EDIT ANYTHING ABOVE


    // Your code ends here -- DO NOT EDIT ANYTHING BELOW
}