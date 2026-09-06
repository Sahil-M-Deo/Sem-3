#include "common.h"
#include "inversion.h"

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

        vector<char> seq(n);
        for (int i = 0; i < n; i++) {
            cin >> seq[i];
        }

        int l, r;
        cin >> l >> r;

        reverse_segment(seq, l, r);

        for (int i = 0; i < n; i++) {
            cout << seq[i];
        }
        cout << "\n";
    }

    return 0;
}
