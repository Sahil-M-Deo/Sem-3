# Lab03 - Next Greater Element

## Problem Statement

Take an index `i` of an array `arr`. Its next greater element is the first
value to the right of `arr[i]` that is strictly larger than `arr[i]`. If no
value to the right is larger, the answer for that index is `-1`.

The obvious way to find it is to stand at index `i` and step right until you
meet a larger value. That settles one index, and doing it for all of them
costs `O(n^2)`. Your solution has to run in `O(n)`.

`nextGreaterElement(arr)` returns an array of the same length as `arr`, where
position `i` holds the next greater element of `arr[i]`, or `-1` when there is
none.

## Constraints

- `1 <= Q <= 10` (number of test cases)
- `1 <= n <= 10^5`
- `0 <= arr[i] <= 10^9`

Array values are never negative, so `-1` cannot be mistaken for a real answer.
Every value fits in an `int`.

## Limits

| Function | Time | Auxiliary space |
|---|---|---|
| `nextGreaterElement` | `O(n)` | `O(n)` |

The largest test cases have `n = 10^5`. On an array of that size where the
step-right search always runs off the end, the `O(n^2)` version takes about 24
seconds, so it will not finish the tests.

## Input Format

- Line 1: `Q`
- For each test case: a line with `n`, then a line with `n` values separated by
  spaces.

## Output Format

One line per test case, with the `n` answers separated by spaces.

### Example

#### Sample Input

```
2
4
4 5 2 25
4
13 7 6 12
```

#### Sample Output

```
5 25 25 -1
-1 12 12 -1
```

#### Explanation

In the first array, the first value right of 4 that beats it is 5, and 25 is
the first to beat 5. For 2 it is 25 as well. Nothing lies right of 25, so its
answer is -1.

In the second array, 13 is larger than everything after it, so its answer is
-1. Both 7 and 6 are beaten by 12, and 12 is the last element.

## Hints for `nextGreaterElement`

Read these only if you are stuck.

1. The nested loop rescans the same stretch of the array over and over. Go
   right to left instead, and hold on to the values you have already passed so
   that you never walk over one a second time.
2. Not all of them are worth holding on to. Put them on a stack with the value
   nearest to the current index on top. A value deep in the stack is useless
   once some value above it is at least as large, because any index that could
   have used the deeper one meets the nearer one first.
3. So at index `i`, pop every value on top of the stack that is `<= arr[i]`.
   Those are exactly the ones that `i`, and everything left of `i`, will never
   need. Whatever is left on top is the answer for `i`. An empty stack means
   there is nothing larger to the right, so the answer is `-1`.
4. Push `arr[i]` and carry on. Every value gets pushed once and popped at most
   once, which is where the `O(n)` comes from.

## Your Task

You must write your implementation in the following file:

```text
next-greater-element.cpp
```

Please edit the code only in the marked area (between `Your code starts from here` and `Your code ends here`) and do not edit anywhere else in this file. You must not edit `next-greater-element.h`, `common.h`, or `main.cpp`.

## Directory Structure

```
next-greater-element.cpp   # implement your solution here
next-greater-element.h     # declaration of the function (DO NOT MODIFY)
common.h                   # shared includes (DO NOT MODIFY)
main.cpp                   # reads input, calls your function, prints the answers (DO NOT MODIFY)
README.md                  # this file (DO NOT MODIFY)
Makefile                   # build automation
```
