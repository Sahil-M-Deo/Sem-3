#include "largest-rectangle.h"

long long largestRectangleBruteForce(const vector<int> &heights) {
    // Your code starts from here -- DO NOT EDIT ANYTHING ABOVE
    int n = heights.size();
    long long best = 0;

    for (int i = 0; i < n; i++) {
        int left = i - 1;
        while (left >= 0 && heights[left] >= heights[i]) left--;

        int right = i + 1;
        while (right < n && heights[right] >= heights[i]) right++;

        // The strips between left and right are the ones it covers.
        long long width = right - left - 1;
        best = max(best, width * heights[i]);
    }

    return best;
    // Your code ends here -- DO NOT EDIT ANYTHING BELOW
}

long long largestRectangleOptimal(const vector<int> &heights) {
    // Your code starts from here -- DO NOT EDIT ANYTHING ABOVE
    int n = heights.size();
    long long best = 0;

    // Strips waiting for a shorter strip on their right. Lengths grow upwards.
    stack<int> st;

    for (int i = 0; i <= n; i++) {
        int cur = (i == n) ? 0 : heights[i]; // sentinel of length 0 empties the stack

        while (!st.empty() && heights[st.top()] >= cur) {
            int height = heights[st.top()];
            st.pop();
            // Strip i stops it on the right, the one below it on the left.
            int left = st.empty() ? -1 : st.top();
            long long width = i - left - 1;
            best = max(best, width * height);
        }

        if (i < n) st.push(i);
    }

    return best;
    // Your code ends here -- DO NOT EDIT ANYTHING BELOW
}
