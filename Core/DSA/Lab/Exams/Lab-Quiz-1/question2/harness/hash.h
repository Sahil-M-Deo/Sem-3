//
// Created by rishabh on 8/22/26.
//

#ifndef LINKED_LIST_HASH_H
#define LINKED_LIST_HASH_H

class NestedList {
public:
    struct InnerNode {
        int value;
        InnerNode* next;
        InnerNode(int val) : value(val), next(nullptr) {}
    };

    struct OuterNode {
        InnerNode* head;
        OuterNode* next;
        OuterNode() : head(nullptr), next(nullptr) {}
    };

    OuterNode* outerHead;
    int N;

    NestedList(int size);
    ~NestedList();

    // Declarations for functions defined in hash.cpp
    void insert(int val);
    bool search(int val) const;

    void display() const;
};

#endif //LINKED_LIST_HASH_H
