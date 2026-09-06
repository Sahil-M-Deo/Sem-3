#pragma once
#include "common.h"

// A BST node, following the structure sketched in Lecture 12: parent
// pointers are maintained so that a node can be detached in O(1) once you
// already have a pointer to it.
struct node {
    int key;
    node *parent, *left, *right;
    node(int k, node *p) : key(k), parent(p), left(nullptr), right(nullptr) {}
};

class tree {
private:
    node *root;

    // Which pointer in the tree currently records p: root, or the
    // relevant child pointer of p->parent. (Lecture 12.)
    node *&where(node *p);

    // Smallest node in the subtree rooted at p (p != nullptr). (Lecture 12.)
    node *minimum(node *p);

    // Detaches p from the tree without deleting it (p != nullptr). (Lecture 12.)
    void detach(node *p);

    // Iteratively frees every node in the tree (used by the destructor).
    void destroyAll();

public:
    tree();
    ~tree();

    tree(const tree &) = delete;
    tree &operator=(const tree &) = delete;

    // Returns the node holding `key`, or nullptr if it isn't in the tree.
    node *find(int key) const;

    // Standard BST insertion (Lecture 12), modified to ignore duplicates:
    // if `v` is already present, the tree is left unchanged.
    void insert(int v);

    // Detaches and deletes p (p != nullptr). (Lecture 12.)
    void erase(node *p);

    // If x is in the tree, replace it with y: if y can be written directly
    // into the node currently holding x without breaking the BST property,
    // just overwrite it there; otherwise remove x and insert y instead. If
    // x is not in the tree, nothing happens. The tree never holds
    // duplicate keys, so if y is already present elsewhere, x is simply
    // removed and y is not inserted again.
    void searchReplace(int x, int y);

    // Level-order (breadth-first, left to right) traversal of the current
    // keys -- used by main.cpp to print the tree's shape after each query.
    vector<int> levelOrder() const;
};
