# Lab04 - Median of a Row-Wise Sorted Matrix

## Problem Statement

Given a matrix in which every row is sorted in non-decreasing order, find its
median. The matrix has an odd number of elements, so the median is the element
at the middle position after all elements are placed in sorted order.

You must not flatten the matrix and sort all of its elements. Use the sorted
order already present in each row to solve the problem efficiently.

`findMedian(matrix)` returns the median element of `matrix`.

## Constraints

- `1 <= Q <= 10` (number of test cases)
- `1 <= rows, cols <= 10^3`
- `rows * cols` is odd
- `1 <= rows * cols <= 10^5`
- `-10^9 <= matrix[i][j] <= 10^9`
- Every row is sorted in non-decreasing order.

## Limits

| Function | Time | Auxiliary space |
|---|---|---|
| `findMedian` | `O(rows * log(cols) * log(value range))` | `O(1)` |

For each candidate value, count how many values in each sorted row are at most
that candidate. This count can be found with binary search. The expected
solution then binary-searches the answer value itself.

## Input Format

- Line 1: `Q`
- For each test case: one line containing `rows` and `cols`
- The next `rows` lines each contain `cols` space-separated matrix values.

## Output Format

For each test case, print the median on its own line.

### Example

#### Sample Input

```text
2
3 3
1 4 9
2 5 6
3 7 8
3 3
1 3 8
2 3 4
1 2 5
```

#### Sample Output

```text
5
3
```

#### Explanation

For the first matrix, the globally sorted order is:

```text
1 2 3 4 5 6 7 8 9
```

The middle element is `5`.

For the second matrix, the globally sorted order is:

```text
1 1 2 2 3 3 4 5 8
```

The middle element is `3`.

## Hints for `findMedian`

Read these only if you are stuck.

1. The median is the element with exactly `(rows * cols) / 2` elements before
   it in globally sorted order.
2. The smallest possible answer is the first element of some row; the largest
   possible answer is the last element of some row.
3. For a candidate `mid`, use `upper_bound` in every row to count the elements
   that are `<= mid`.
4. If that count is at most half the elements, the median must be larger;
   otherwise, `mid` may be the answer or the answer may be smaller.

## Your Task

You must write your implementation in the following file:

```text
median-of-matrix.cpp
```

Please edit the code only in the marked area (between `Your code starts from
here` and `Your code ends here`) and do not edit anywhere else in this file.
You must not edit `median-of-matrix.h`, `common.h`, or `main.cpp`.

## Directory Structure

```text
median-of-matrix.cpp   # implement your solution here
median-of-matrix.h     # declaration of the function (DO NOT MODIFY)
common.h               # shared includes (DO NOT MODIFY)
main.cpp               # reads input, calls your function, prints the answer (DO NOT MODIFY)
README.md              # this file (DO NOT MODIFY)
Makefile               # build automation
```
