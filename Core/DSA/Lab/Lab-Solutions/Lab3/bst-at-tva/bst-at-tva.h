#pragma once
#include "common.h"

// Inserts `key` into the BST rooted at `root` using standard BST insertion
// (smaller goes left, larger goes right) and returns the root of the
// resulting tree (the same `root`, unless the tree was empty).
Node *insert(Node *root, int key);

// Searches for `key` in the BST rooted at `root`. Sets `found` to whether
// the key exists in the tree, and returns the number of nodes examined
// along the way -- inclusive of the node where `key` is found, or
// inclusive of the last node checked before the search falls off the tree.
int search(Node *root, int key, bool &found);
