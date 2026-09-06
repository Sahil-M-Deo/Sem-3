#include "search-replace.h"

// --- Helper Functions (Already Implemented, straight from Lecture 12) ---

tree::tree() : root(nullptr) {}

tree::~tree() {
    destroyAll();
}

void tree::destroyAll() {
    // Iterative so it doesn't blow the stack on a deeply skewed tree.
    vector<node *> stack;
    if (root != nullptr) stack.push_back(root);
    while (!stack.empty()) {
        node *p = stack.back();
        stack.pop_back();
        if (p->left != nullptr) stack.push_back(p->left);
        if (p->right != nullptr) stack.push_back(p->right);
        delete p;
    }
    root = nullptr;
}

node *&tree::where(node *p) {
    if (p == root) return root;
    return (p->parent->left == p) ? p->parent->left : p->parent->right;
}

node *tree::minimum(node *p) {
    while (p->left) p = p->left;
    return p;
}

void tree::detach(node *p) {
    node *&pp = where(p);
    if (!p->left || !p->right) { // at least one child slot is empty
        pp = p->left ? p->left : p->right;
        if (pp) pp->parent = p->parent;
        return;
    }
    node *q = minimum(p->right); // successor: q->left == nullptr
    detach(q);                    // so this recursive call hits the case above
    pp = q;
    p->left->parent = q; // p->left != nullptr
    if (p->right) p->right->parent = q; // detach(q) may have set p->right = nullptr
    q->left = p->left;
    q->right = p->right;
    q->parent = p->parent;
    p->left = p->right = p->parent = nullptr;
}

void tree::erase(node *p) {
    if (!p) return;
    detach(p);
    delete p;
}

node *tree::find(int key) const {
    node *curr = root;
    while (curr != nullptr && curr->key != key) {
        curr = (key < curr->key) ? curr->left : curr->right;
    }
    return curr;
}

void tree::insert(int v) {
    // search for the parent whose child the new node will be
    node *parent = nullptr;
    for (node *curr = root; curr != nullptr;) {
        if (curr->key == v) return; // already present -- ignore duplicates
        parent = curr;
        if (v < curr->key) curr = curr->left;
        else curr = curr->right;
    }
    // create and attach the new node
    node *newnode = new node(v, parent);
    if (parent == nullptr) root = newnode; // was an empty tree
    else if (v < parent->key) parent->left = newnode;
    else parent->right = newnode;
}

vector<int> tree::levelOrder() const {
    vector<int> result;
    vector<node *> queue;
    if (root != nullptr) queue.push_back(root);
    size_t head = 0;
    while (head < queue.size()) {
        node *p = queue[head++];
        result.push_back(p->key);
        if (p->left != nullptr) queue.push_back(p->left);
        if (p->right != nullptr) queue.push_back(p->right);
    }
    return result;
}

// --- Student Implementation Section ---

void tree::searchReplace(int x, int y) {
    // Your code starts from here -- DO NOT EDIT ANYTHING ABOVE

    node *p = find(x);
    if (p == nullptr) return; // x isn't in the tree -- nothing to do
    if (x == y) return;       // already the value we want -- nothing to do

    node *existing = find(y);
    if (existing != nullptr) {
        // y is already present elsewhere; no duplicates allowed, so x is
        // just removed and y is NOT inserted again.
        erase(p);
        return;
    }

    // y isn't present anywhere else. Figure out whether it can be written
    // directly into p's slot: that's possible exactly when y falls
    // strictly between p's in-order predecessor and successor in the
    // whole tree (the values that would remain p's neighbours either way).

    // Predecessor: largest key smaller than p's. If p has a left subtree,
    // it's the max of that subtree; otherwise walk up until we take a
    // right turn (i.e. we were someone's right child).
    bool hasLower = false;
    int lower = 0;
    if (p->left != nullptr) {
        node *q = p->left;
        while (q->right) q = q->right;
        hasLower = true;
        lower = q->key;
    } else {
        node *child = p, *anc = p->parent;
        while (anc != nullptr && anc->left == child) {
            child = anc;
            anc = anc->parent;
        }
        if (anc != nullptr) {
            hasLower = true;
            lower = anc->key;
        }
    }

    // Successor: smallest key larger than p's. Symmetric to the above.
    bool hasUpper = false;
    int upper = 0;
    if (p->right != nullptr) {
        hasUpper = true;
        upper = minimum(p->right)->key;
    } else {
        node *child = p, *anc = p->parent;
        while (anc != nullptr && anc->right == child) {
            child = anc;
            anc = anc->parent;
        }
        if (anc != nullptr) {
            hasUpper = true;
            upper = anc->key;
        }
    }

    bool fits = (!hasLower || y > lower) && (!hasUpper || y < upper);

    if (fits) {
        p->key = y; // overwrite in place -- no structural change needed
    } else {
        erase(p);
        insert(y);
    }

    // Your code ends here -- DO NOT EDIT ANYTHING BELOW
}
