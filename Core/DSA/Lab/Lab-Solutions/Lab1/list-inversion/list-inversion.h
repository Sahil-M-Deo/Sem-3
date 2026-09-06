#pragma once
#include "common.h"

struct Node {
    char base;
    Node *next;
};

// Reverses the entire singly linked list in place, by relinking the existing
// nodes so that the order of the bases is flipped end to end. Returns the head
// of the resulting list (the node that used to be last is now first).
// A list of length 1 is left unchanged (but the same head is returned).
Node* reverse_list(Node* head);
