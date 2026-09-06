#include "bst-at-tva.h"

Node *insert(Node *root, int key) {
    // Your code starts from here -- DO NOT EDIT ANYTHING ABOVE
    if (root == nullptr) {
        return new Node(key);
    }

    Node *cur = root;
    while (true) {
        if (key < cur->key) {
            if (cur->left == nullptr) {
                cur->left = new Node(key);
                break;
            }
            cur = cur->left;
        } else {
            if (cur->right == nullptr) {
                cur->right = new Node(key);
                break;
            }
            cur = cur->right;
        }
    }
    return root;
    // Your code ends here -- DO NOT EDIT ANYTHING BELOW
}

int search(Node *root, int key, bool &found) {
    // Your code starts from here -- DO NOT EDIT ANYTHING ABOVE
    int examined = 0;
    Node *cur = root;
    while (cur != nullptr) {
        examined++;
        if (key == cur->key) {
            found = true;
            return examined;
        }
        cur = (key < cur->key) ? cur->left : cur->right;
    }
    found = false;
    return examined;
    // Your code ends here -- DO NOT EDIT ANYTHING BELOW
}
