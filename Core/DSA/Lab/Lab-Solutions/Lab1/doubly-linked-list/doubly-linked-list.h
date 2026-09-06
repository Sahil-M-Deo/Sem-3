#pragma once
#include "common.h"

// A node of the list. 
struct node {
    int val;
    node *next, *prev;
};


class list {
private:
    node sentinel;   // sentinel.next is head, sentinel.prev is tail
    int  sz;         // number of elements (does not count the sentinel)
    bool flipped;    // true => next and prev are to be read in the opposite sense

public:

    // An empty list: the sentinel points at itself. 
    list() { sentinel.val = 0; sentinel.next = sentinel.prev = &sentinel; sz = 0; flipped = false; }

    // end() points at the sentinel itself, in both directions of traversal.
    node* end() { return &sentinel; }

    // Insert a new node holding v immediately before p in *logical* order.
    // p may be end(), in which case this appends at the back.
       void insert_before(node* p, int v);

    // Remove p from the list and free it.
    void erase(node* p);

    // Copying a list would copy the sentinel and leave two lists owning the
    // same nodes, so it is switched off. Pass lists by reference.
    list(const list&) = delete;
    list& operator=(const list&) = delete;

    // ---------- to implement: the exercise ----------

    // Free every node of the list (but not the sentinel -- it is a member, not
    // a heap allocation).
    ~list();

    // The first element in logical order. Equals end() when the list is empty.
    node* begin();

    node* next(node* p);

    node* prev(node* p);

    // Number of elements. Must be O(1): return the maintained counter, do not
    // walk the list.
    int size() const;

    // Reverse the list. Must be O(1): toggle the flag, nothing else.
    void reverse();

    // Append every element of other to the back of the first list, keeping their
    // order, and leave 'other' a valid empty list. Both lists may have their
    // flags set either way, so append in other's logical order. Self-join
    // (L.join(L)) must be a no-op. 
    void join(list& other);

    // Print the elements in logical order, space separated, followed by a
    // newline. An empty list prints just the newline.
    void print();

    // The first node in logical order whose val is v, or end() if there is
    // none. Must not modify the list.
    node* find(int v);
};
