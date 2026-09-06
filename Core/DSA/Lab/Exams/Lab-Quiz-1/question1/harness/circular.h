#ifndef CIRCULAR_H
#define CIRCULAR_H

#include <iostream>

struct node {
    int data;
    node* next;
    node* prev;
    node(int val = 0) : data(val), next(this), prev(this) {}
};

struct list {
    node sentinel;

    // Inline Constructor
    list() : sentinel(0) {
        sentinel.next = &sentinel;
        sentinel.prev = &sentinel;
    }

    // Inline Destructor
    ~list() {
        clear();
    }

    // Prevent deep copy issues during simple assignment
    list(const list&) = delete;
    list& operator=(const list&) = delete;

    // Inline Move Constructor
    list(list&& other) noexcept : sentinel(0) {
        sentinel.next = &sentinel;
        sentinel.prev = &sentinel;

        if (other.sentinel.next != &other.sentinel) {
            node* first = other.sentinel.next;
            node* last = other.sentinel.prev;

            this->sentinel.next = first;
            this->sentinel.prev = last;
            first->prev = &this->sentinel;
            last->next = &this->sentinel;

            other.sentinel.next = &other.sentinel;
            other.sentinel.prev = &other.sentinel;
        }
    }

    // Inline Move Assignment
    list& operator=(list&& other) noexcept {
        if (this != &other) {
            clear();
            if (other.sentinel.next != &other.sentinel) {
                node* first = other.sentinel.next;
                node* last = other.sentinel.prev;

                this->sentinel.next = first;
                this->sentinel.prev = last;
                first->prev = &this->sentinel;
                last->next = &this->sentinel;

                other.sentinel.next = &other.sentinel;
                other.sentinel.prev = &other.sentinel;
            }
        }
        return *this;
    }

    // Inline Helper Methods
    void push_back(int val) {
        node* newNode = new node(val);
        node* last = sentinel.prev;

        last->next = newNode;
        newNode->prev = last;
        newNode->next = &sentinel;
        sentinel.prev = newNode;
    }

    void clear() {
        node* current = sentinel.next;
        while (current != &sentinel) {
            node* temp = current;
            current = current->next;
            delete temp;
        }
        sentinel.next = &sentinel;
        sentinel.prev = &sentinel;
    }

    void display() const {
        const node* current = sentinel.next;
        while (current != &sentinel) {
            std::cout << current->data << " ";
            current = current->next;
        }
        std::cout << "\n";
    }

    node* find(int val) const {
        const node* current = sentinel.next;
        while (current != &sentinel) {
            if (current->data == val) {
                return const_cast<node*>(current);
            }
            current = current->next;
        }
        return nullptr;
    }

    // Declarations for methods implemented in circular.cpp
    void merge(list& other);
    list split(node* p);
};

#endif // CIRCULAR_H