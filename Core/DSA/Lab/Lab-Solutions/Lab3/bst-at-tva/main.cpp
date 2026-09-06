#include "common.h"
#include "bst-at-tva.h"

// Iterative so it doesn't blow the stack on a deeply skewed tree.
static void free_tree(Node *root) {
    vector<Node *> stack;
    if (root != nullptr) stack.push_back(root);
    while (!stack.empty()) {
        Node *node = stack.back();
        stack.pop_back();
        if (node->left != nullptr) stack.push_back(node->left);
        if (node->right != nullptr) stack.push_back(node->right);
        delete node;
    }
}

int main(int argc, char **argv) {
    if (argc > 1) {
        if (freopen(argv[1], "r", stdin) == nullptr) {
            std::cerr << "Error: Could not open input file " << argv[1] << std::endl;
            return 1;
        }
    }
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);

    int t;
    cin >> t;
    while (t--) {
        int n;
        cin >> n;
        Node *root = nullptr;
        for (int i = 0; i < n; i++) {
            int key;
            cin >> key;
            root = insert(root, key);
        }

        int q;
        cin >> q;
        for (int i = 0; i < q; i++) {
            int key;
            cin >> key;
            bool found = false;
            int examined = search(root, key, found);
            if (found) {
                cout << "found after examining " << examined << " nodes\n";
            } else {
                cout << "not found after examining " << examined << " nodes\n";
            }
        }

        free_tree(root);
    }
    return 0;
}
