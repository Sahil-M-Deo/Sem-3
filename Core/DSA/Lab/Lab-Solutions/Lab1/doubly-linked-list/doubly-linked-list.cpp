#include "doubly-linked-list.h"

// ============================================================================
// GIVEN -- do not edit. exanple given to show how to
// maintain sz and to respect the flipped flag. Read them: they show exactly
// what "incorporate the flag" means, and the functions you write must follow
// the same convention.
// ============================================================================

void list::insert_before(node* p, int v) {
    if (!flipped) {
        // logical order == physical order: put the new node between
        // p->prev and p, exactly as on the slide.
        node* tmp = new node{v, p, p->prev};
        p->prev->next = tmp;
        p->prev = tmp;
    } else {
        // logical order is reversed: "just before p" logically means
        // "just after p" physically.
        node* tmp = new node{v, p->next, p};
        p->next->prev = tmp;
        p->next = tmp;
    }
    sz++;
}

void list::erase(node* p) {
    // Unlinking is symmetric, so this needs no flag test: whichever way the
    // list is being read, p's two physical neighbours become each other's.
    p->prev->next = p->next;
    p->next->prev = p->prev;
    delete p;
    sz--;
}

// ============================================================================
// YOUR CODE -- implement each body between its two marker comments.
// ============================================================================

list::~list() {
    // Your code starts from here -- DO NOT EDIT ANYTHING ABOVE
    // Physical traversal is fine here: we want to free every node exactly
    // once, and the flag only affects the order we would visit them in.
    node* p = sentinel.next;
    while (p != &sentinel) {
        node* q = p->next;
        delete p;
        p = q;
    }
    sentinel.next = sentinel.prev = &sentinel;
    sz = 0;
    // Your code ends here -- DO NOT EDIT ANYTHING BELOW
}

node* list::begin() {
    // Your code starts from here -- DO NOT EDIT ANYTHING ABOVE
    return flipped ? sentinel.prev : sentinel.next;
    // Your code ends here -- DO NOT EDIT ANYTHING BELOW
}

node* list::next(node* p) {
    // Your code starts from here -- DO NOT EDIT ANYTHING ABOVE
    return flipped ? p->prev : p->next;
    // Your code ends here -- DO NOT EDIT ANYTHING BELOW
}

node* list::prev(node* p) {
    // Your code starts from here -- DO NOT EDIT ANYTHING ABOVE
    return flipped ? p->next : p->prev;
    // Your code ends here -- DO NOT EDIT ANYTHING BELOW
}

int list::size() const {
    // Your code starts from here -- DO NOT EDIT ANYTHING ABOVE
    return sz;
    // Your code ends here -- DO NOT EDIT ANYTHING BELOW
}

void list::reverse() {
    // Your code starts from here -- DO NOT EDIT ANYTHING ABOVE
    // No node is touched: reading the list the other way round is the reversal.
    flipped = !flipped;
    // Your code ends here -- DO NOT EDIT ANYTHING BELOW
}

void list::join(list& other) {
    // Your code starts from here -- DO NOT EDIT ANYTHING ABOVE
    if (this == &other) return;   // L.join(L) does nothing

    // Move other's nodes over one at a time, in other's logical order, and
    // splice each one in just before our sentinel (i.e. at our logical back).
    // Nothing is allocated and nothing is freed -- the nodes change owner.
    node* p = other.begin();
    while (p != other.end()) {
        node* nxt = other.next(p);   // read the link BEFORE we overwrite it
        node* s = &sentinel;
        if (!flipped) {
            p->next = s; p->prev = s->prev;
            s->prev->next = p; s->prev = p;
        } else {
            p->prev = s; p->next = s->next;
            s->next->prev = p; s->next = p;
        }
        sz++;
        p = nxt;
    }

    // Leave other a valid empty list.
    other.sentinel.next = other.sentinel.prev = &other.sentinel;
    other.sz = 0;
    other.flipped = false;
    // Your code ends here -- DO NOT EDIT ANYTHING BELOW
}

void list::print() {
    // Your code starts from here -- DO NOT EDIT ANYTHING ABOVE
    bool first = true;
    for (node* p = begin(); p != end(); p = next(p)) {
        if (!first) cout << ' ';
        cout << p->val;
        first = false;
    }
    cout << "\n";
    // Your code ends here -- DO NOT EDIT ANYTHING BELOW
}

node* list::find(int v) {
    // Your code starts from here -- DO NOT EDIT ANYTHING ABOVE
    for (node* p = begin(); p != end(); p = next(p))
        if (p->val == v) return p;
    return end();   // not found -- same convention as std::list
    // Your code ends here -- DO NOT EDIT ANYTHING BELOW
}
