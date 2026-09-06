#include "common.h"
#include "search-replace.h"

static void print_level_order(const tree &t) {
    vector<int> levels = t.levelOrder();
    for (size_t i = 0; i < levels.size(); i++) {
        if (i != 0) cout << " ";
        cout << levels[i];
    }
    cout << "\n";
}

int main(int argc, char** argv) {
    if (argc > 1) {
        if (freopen(argv[1], "r", stdin) == nullptr) // cin redirects to file argv[1]
        {
            std::cerr << "Error: Could not open input file " << argv[1] << std::endl;
            return 1;
        }
    }
    ios_base::sync_with_stdio(false);
    cin.tie(nullptr);

    int t;
    cin >> t;
    while (t--) {
        tree bst;

        int n;
        cin >> n;
        for (int i = 0; i < n; i++) {
            int key;
            cin >> key;
            bst.insert(key);
        }

        int q;
        cin >> q;
        for (int i = 0; i < q; i++) {
            int x, y;
            cin >> x >> y;
            bst.searchReplace(x, y);
            print_level_order(bst);
        }
    }

    return 0;
}
