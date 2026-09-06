#include "median-of-matrix.h"

int findMedian(const vector<vector<int>>& matrix) {
    // Your code starts from here -- DO NOT EDIT ANYTHING ABOVE
    int low = matrix[0][0];
    int high = matrix[0].back();
    int smallerCount = matrix.size() * matrix[0].size() / 2;

    for (const vector<int>& row : matrix) {
        low = min(low, row.front());
        high = max(high, row.back());
    }

    while (low < high) {
        int mid = low + (high - low) / 2;
        int count = 0;

        for (const vector<int>& row : matrix) {
            count += upper_bound(row.begin(), row.end(), mid) - row.begin();
        }

        if (count <= smallerCount) {
            low = mid + 1;
        } else {
            high = mid;
        }
    }

    return low;
    // Your code ends here -- DO NOT EDIT ANYTHING BELOW
}
