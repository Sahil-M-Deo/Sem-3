# Lab04 - Largest Rectangle in a Patchwork of Strips

## Problem Statement

`n` strips of cloth are sewn side by side into a patchwork. Strip `i` is 1 unit
wide and `heights[i]` units long, and one end of every strip sits on a straight
edge. Report the area of the largest rectangle that can be cut out of the
patchwork with one side on that edge.

A rectangle can be no taller than the shortest strip it covers. Grow one until
it is stuck. It stops widening once the strips just outside both of its ends are
strictly shorter than it is tall, and it stops growing taller once some strip it
covers is exactly as long as it is tall. Call such a rectangle maximal, and call
the strip that stops it from growing taller its limiting strip.

The largest rectangle is maximal, so there is one candidate per strip. Take
strip `i` as the limiting strip: the rectangle is `heights[i]` tall and runs up
to the closest strictly shorter strip on either side. Count the two ends of the
patchwork as strips of length 0. The largest of those `n` areas is the answer.

Walking outwards from strip `i` to find those two strips takes `O(n)` steps, so
`O(n^2)` for all of them, and that cost is real: when every strip is the same
length each walk runs the whole way. That is the first function. The second has
to give the same answer in `O(n)`.

`largestRectangleBruteForce(heights)` does that walk from every strip.

`largestRectangleOptimal(heights)` returns the same number in `O(n)` time.

## Constraints

- `1 <= Q <= 10` (number of test cases)
- `1 <= n <= 10^5`
- `0 <= heights[i] <= 10^4`

The largest area is `10^5 * 10^4 = 10^9`. That fits in an `int`, but only just,
so both functions return `long long`.

## Limits

| Function | Time | Auxiliary space |
|---|---|---|
| `largestRectangleBruteForce` | `O(n^2)` | `O(1)` |
| `largestRectangleOptimal` | `O(n)` | `O(n)` |

The brute force is quadratic by design, so `main.cpp` calls it only when
`n <= 5000` and prints `skipped` above that.

The largest test cases have `n = 10^5`, where the quadratic scan takes about 40
seconds. The second function has to be the linear one.

## Input Format

- Line 1: `Q`
- For each test case: a line with `n`, then a line with the `n` lengths
  separated by spaces.

## Output Format

One line per test case: the brute force area, a space, then the optimal area.
The two have to match. When `n > 5000` the brute force is not called and the
first value is the word `skipped`.

### Example

#### Sample Input

```
2
6
2 1 5 6 2 3
2
2 4
```

#### Sample Output

```
10 10
4 4
```

#### Explanation

In the first case the rectangle covers the strips of lengths 5 and 6. Its
limiting strip is the one of length 5, and the strips just outside it are 1 and
2, so it is 5 tall and 2 wide.

In the second case a rectangle 2 tall covers both strips, and a rectangle 4 tall
covers the second strip alone. Both have area 4.

## Hints for `largestRectangleOptimal`

Read these only if you are stuck.

1. The brute force keeps rescanning strips. Scan once from left to right
   instead, keeping the strips you have passed on a stack. Each strip is then
   pushed once and popped once.
2. Push indices, and keep their lengths strictly increasing up the stack: before
   pushing strip `i`, pop every entry at least as long as `heights[i]`. What is
   left on top is the closest shorter strip to the left of `i`.
3. Read that the other way round: an entry is popped by the first strip to its
   right that is not longer than it. So when an entry is popped you know both of
   its ends and can work out its area. Check what this gives you when two strips
   are the same length.
4. A sentinel of length 0 after the last strip clears out whatever is still on
   the stack.

## Your Task

You must write your implementation in the following file:

```text
largest-rectangle.cpp
```

Please edit the code only in the marked areas (between `Your code starts from here` and `Your code ends here`) and do not edit anywhere else in this file. You must not edit `largest-rectangle.h`, `common.h`, or `main.cpp`.

## Directory Structure

```
largest-rectangle.cpp   # implement your solution here
largest-rectangle.h     # declarations of the two functions (DO NOT MODIFY)
common.h                # shared includes (DO NOT MODIFY)
main.cpp                # reads input, calls both functions, prints the areas (DO NOT MODIFY)
README.md               # this file (DO NOT MODIFY)
Makefile                # build automation
```
