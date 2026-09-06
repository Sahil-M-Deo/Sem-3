#include "inversion.h"

void reverse_segment(vector<char> &v, int L, int R) {
    // Your code starts from here -- DO NOT EDIT ANYTHING ABOVE
    while (L < R) {
        char temp = v[L];
        v[L] = v[R];
        v[R] = temp;
        L++;
        R--;
    }
    // Your code ends here -- DO NOT EDIT ANYTHING BELOW
}
