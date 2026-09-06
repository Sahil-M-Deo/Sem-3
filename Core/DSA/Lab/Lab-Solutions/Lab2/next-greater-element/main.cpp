#include "common.h"
#include "next-greater-element.h"

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

        vector<int> arr(n);
        for (int i = 0; i < n; i++) {
            cin >> arr[i];
        }

        vector<int> ans = nextGreaterElement(arr);
        for (int i = 0; i < n; i++) {
            cout << ans[i] << (i + 1 == n ? '\n' : ' ');
        }
    }
    return 0;
}
