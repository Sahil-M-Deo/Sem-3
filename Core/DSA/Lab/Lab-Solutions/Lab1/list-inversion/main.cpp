#include "common.h"
#include "list-inversion.h"
// Your code starts from here -- DO NOT EDIT ANYTHING ABOVE
// Your code ends here -- DO NOT EDIT ANYTHING BELOW

int main(int argc, char** argv) {
    if (argc > 1) {
        if (freopen(argv[1], "r", stdin) == nullptr) // cin redirects to file argv[1]
        {
            std::cerr << "Error: Could not open input file " << argv[1] << std::endl;
            return 1;
        }
    }
    // Fast I/O is required for long sequences
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);

    int q;
    cin >> q;
    while (q--) {
        int n;
        cin >> n;

        Node *head = nullptr;
        Node *tail = nullptr;
        for (int i = 0; i < n; i++) {
            char c;
            cin >> c;
            Node *node = new Node{c, nullptr};
            if (head == nullptr) {
                head = node;
                tail = node;
            } else {
                tail->next = node;
                tail = node;
            }
        }

        head = reverse_list(head);

        Node *cur = head;
        while (cur != nullptr) {
            cout << cur->base;
            cur = cur->next;
        }
        cout << "\n";

        // Free the list
        cur = head;
        while (cur != nullptr) {
            Node *nxt = cur->next;
            delete cur;
            cur = nxt;
        }
    }
    return 0;
}
