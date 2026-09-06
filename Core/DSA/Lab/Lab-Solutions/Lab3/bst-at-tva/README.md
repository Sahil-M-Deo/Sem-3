# Lab 04 - The Variant Registry

[Ctrl + Shift + V to see the rendered version of this file]

## Background (You can skip this if you want >:()

Loki is just been reassigned to the Archive, the vault three floors down
where every variant case file at the Time Variance Authority ends up.
On the instructions of the time keepers, every variant still gets pruned
the moment they tries to defy "The Sacred Timeline", and every prune 
still gets a case number, issued once, in order, never reused. 
When a new anomaly pings the timeline, they want to know immediately
whether this variant is already on file, and exactly how many case
files it took to be sure. You need to help Loki in the following task.
Case numbers get filed into a binary search tree, one at a time, in the
order they were first opened. Once the registry is built, a batch of find
instructions runs against it, and for each one you need to know whether the case
number is on file and exactly how many entries it took to be sure.

## Problem Statement

You are given a sequence of `N` distinct integer case numbers.
Insert them into a binary search tree one after another, in the order
given, using standard BST insertion (no rebalancing).

(Probable Thoughts - Wdym by "no rebalancing", I don't even know how to rebalance??
My response - Dw, It will be covered in the lectures soon.
)

Then, for each of `Q` pings, report whether the queried case number exists in the tree, and how
many nodes were examined while looking for it.

Reading the input, building the tree by repeatedly calling your `insert`,
running the queries through your `search`, and printing the results are
already done for you in `main.cpp`. You write `insert` and `search`.

## Your Task

Write your code in `bst.cpp`, inside the body of each function:

```cpp
Node *insert(Node *root, int key) {
    // Your code starts from here -- DO NOT EDIT ANYTHING ABOVE
    ...
    // Your code ends here -- DO NOT EDIT ANYTHING BELOW
}

int search(Node *root, int key, bool &found) {
    // Your code starts from here -- DO NOT EDIT ANYTHING ABOVE
    ...
    // Your code ends here -- DO NOT EDIT ANYTHING BELOW
}
```

Do not delete or move the marker comments, do not write anything outside
them, and do not change any other file.

## Directory Structure

```
bst-at-tva.cpp          # write your solution here
bst-at-tva.h                   # declaration of insert and search (DO NOT MODIFY)
common.h                # the headers and Node struct you are allowed to use (DO NOT MODIFY)
main.cpp                # reads input, calls your functions, prints the result (DO NOT MODIFY)
README.md               # this file (DO NOT MODIFY)
Makefile                # build automation
generate_testcases.py   # regenerates tests/ from an independent reference implementation
```

## Rules of the Problem

### Function Specification

#### `Node *insert(Node *root, int key)`

Inserts `key` into the BST rooted at `root`: if `key` is less than a node's
key go left, otherwise go right, until an empty spot is found. Returns the
root of the resulting tree (the same `root`, unless the tree was empty, in
which case the new node becomes the root).

#### `int search(Node *root, int key, bool &found)`

Searches for `key` starting at `root`. Sets `found` to `true` if `key` is
somewhere in the tree, `false` otherwise. Returns the number of nodes
examined along the way: this counts the node currently being compared
against at every step, including the node where `key` is found (so finding
it at the root counts as `1`), or including the last node checked
immediately before the search runs off the tree into an empty child.

### Constraints

| Resource | Limit |
|---|---|
| Time to build the tree | `O(Nlog(N))` amortized|
| Time per search | `O(H)`, where `H` is the current height of the tree |
| Auxiliary space | `O(1)` extra per call (besides the tree itself) |

- **Careful with recursion.** A tree built from a sorted (or reverse-sorted)
  insertion sequence is a straight chain `N` nodes deep. A recursive
  `insert` or `search` will recurse that deep too, and on a big enough input
  that will overflow the call stack and crash, even though the algorithm
  itself is correct. Prefer a `while` loop that walks down the tree.

### Input and Output (handled by main.cpp)

Input format:

```
T
N_1
key_1 key_2 ... key_N1
Q_1
query_1 query_2 ... query_Q1
N_2
...
```

- `T` is the number of independent test cases
- For each test case: `N`, then the `N` distinct case numbers to insert (in
  order) on one line, then `Q`, then the `Q` queries to run against that
  tree (in order) on one line

Output format: for each query, in order, one line reading either

```
found after examining X nodes
```

or

```
not found after examining X nodes
```

Example input:

```
1
7
50 30 70 20 40 60 80
5
60 25 50 90 40
```

Example output:

```
found after examining 3 nodes
not found after examining 3 nodes
found after examining 1 nodes
not found after examining 3 nodes
found after examining 3 nodes
```

Walkthrough: inserting `50 30 70 20 40 60 80` in that order builds

```
          50
        /    \
      30      70
     /  \    /  \
   20   40  60   80
```

- query `60`: examine `50` (1, 60>50, go right), `70` (2, 60<70, go left),
  `60` (3, match) -> `found after examining 3 nodes`
- query `25`: examine `50` (1, go left), `30` (2, go left), `20` (3,
  25>20 but `20` has no right child) -> `not found after examining 3 nodes`
- query `50`: examine `50` (1, match) -> `found after examining 1 nodes`
- query `90`: examine `50` (1), `70` (2), `80` (3, 90>80 but no right
  child) -> `not found after examining 3 nodes`
- query `40`: examine `50` (1), `30` (2), `40` (3, match) -> `found after
  examining 3 nodes`

### Assumptions

- `1 <= N, Q <= 2*10^5` per test case (dw I know the worst case time complexity is O(n^2), such testcases will have smaller N and Q)
- the `N` values inserted into any one tree are guaranteed distinct
- a query value may or may not appear among the inserted keys, and the same
  query value may appear more than once across the `Q` queries in a test
  case
- Hehe, the phrasing `examining X nodes` is used verbatim regardless of whether
  `X` is `1`. DO NOT special-case the grammar

## Loki will return
