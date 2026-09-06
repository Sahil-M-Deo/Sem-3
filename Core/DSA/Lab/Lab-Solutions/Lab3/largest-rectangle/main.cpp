#include "common.h"
#include "largest-rectangle.h"

// The brute force is quadratic, so it only runs on the smaller inputs.
const int BRUTE_FORCE_LIMIT = 5000;

int main(int argc, char **argv) {
    if (argc > 1) {
        if (freopen(argv[1], "r", stdin) == nullptr) // cin redirects to file argv[1]
        {
            std::cerr << "Error: Could not open input file " << argv[1] << std::endl;
            return 1;
        }
    }
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);

    int q;
    cin >> q;
    while (q--) {
        int n;
        cin >> n;

        vector<int> heights(n);
        for (int i = 0; i < n; i++) {
            cin >> heights[i];
        }

        if (n <= BRUTE_FORCE_LIMIT) {
            cout << largestRectangleBruteForce(heights);
        } else {
            cout << "skipped";
        }
        cout << " " << largestRectangleOptimal(heights) << "\n";
    }
    return 0;
}
