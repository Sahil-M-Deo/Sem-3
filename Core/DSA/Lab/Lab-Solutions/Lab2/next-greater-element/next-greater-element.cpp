#include "next-greater-element.h"

vector<int> nextGreaterElement(const vector<int> &arr) {
    // Your code starts from here -- DO NOT EDIT ANYTHING ABOVE
    int n = arr.size();
    vector<int> ans(n, -1);

    // Values right of i, decreasing from the bottom of the stack to the top.
    stack<int> st;

    for (int i = n - 1; i >= 0; i--) {
        // Too small to answer i, so too small for anything left of i as well.
        while (!st.empty() && st.top() <= arr[i]) st.pop();

        if (!st.empty()) ans[i] = st.top();
        st.push(arr[i]);
    }

    return ans;
    // Your code ends here -- DO NOT EDIT ANYTHING BELOW
}
