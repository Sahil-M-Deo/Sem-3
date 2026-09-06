#include "common.h"
#include "doubly-linked-list.h"
// Your code starts from here -- DO NOT EDIT ANYTHING ABOVE
// Your code ends here -- DO NOT EDIT ANYTHING BELOW

// Every test case works with two lists, numbered 0 and 1, so that join can be
// exercised. Each command names the list it acts on.


static node* at(list& L, int pos) {
    node* p = L.begin();
    for (int i = 0; i < pos && p != L.end(); i++) p = L.next(p);
    return p;
}

// Walks the list backwards, from the last element to the first, using
// prev(). If your prev() or your flag handling is wrong, this line will
// disagree with print() -- that is exactly what it is here to catch.
static void print_backward(list& L) {
    bool first = true;
    for (node* p = L.prev(L.end()); p != L.end(); p = L.prev(p)) {
        if (!first) cout << ' ';
        cout << p->val;
        first = false;
    }
    cout << "\n";
}

int main(int argc, char **argv) {
    if (argc > 1) {
        if (freopen(argv[1], "r", stdin) == nullptr) { // cin redirects to file argv[1]
            std::cerr << "Error: Could not open input file " << argv[1] << std::endl;
            return 1;
        }
    }
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);

    int q;
    cin >> q;
    while (q--) {
        int m;
        cin >> m;

        list L[2];
        for (int i = 0; i < m; i++) {
            string cmd;
            int id;
            cin >> cmd >> id;
            list& A = L[id];

            if (cmd == "push_front") {
                int v; cin >> v;
                A.insert_before(A.begin(), v);
            } else if (cmd == "push_back") {
                int v; cin >> v;
                A.insert_before(A.end(), v);
            } else if (cmd == "insert_before") {
                int p, v; cin >> p >> v;
                A.insert_before(at(A, p), v);
            } else if (cmd == "erase_at") {
                int p; cin >> p;
                A.erase(at(A, p));
            } else if (cmd == "find") {
                // find returns a node*; the driver turns it into an index so
                // that it can be compared against the expected output.
                int v; cin >> v;
                node* r = A.find(v);
                if (r == A.end()) {
                    cout << -1 << "\n";
                } else {
                    int idx = 0;
                    for (node* p = A.begin(); p != r; p = A.next(p)) idx++;
                    cout << idx << "\n";
                }
            } else if (cmd == "size") {
                cout << A.size() << "\n";
            } else if (cmd == "reverse") {
                A.reverse();
            } else if (cmd == "join") {
                int other; cin >> other;
                A.join(L[other]);
            } else if (cmd == "print") {
                A.print();
            } else if (cmd == "rprint") {
                print_backward(A);
            }
        }
        // No manual clean-up here: leaving this scope destroys both lists, and
        // freeing their nodes is your destructor's job.
    }
    return 0;
}
