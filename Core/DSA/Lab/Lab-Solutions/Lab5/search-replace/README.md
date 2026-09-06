# Lab 05 - Search and Replace in a BST

## Problem Statement

You are given a Binary Search Tree (BST) that never contains duplicate
keys. You must implement `searchReplace(x, y)`:

* If `x` is **not** in the tree, do nothing.
* If `x` **is** in the tree, replace it with `y`:
  * If `y` can be written directly into the node currently holding `x`
    without breaking the BST property, just overwrite the key stored in
    that node — the node, its children, and the rest of the tree are left
    untouched.
  * Otherwise, `x` is removed from the tree and `y` is inserted into it,
    using standard BST delete and insert.
* Because the tree never holds duplicates: if `y` is already present
  somewhere else in the tree, `x` is simply deleted and `y` is **not**
  inserted again.

This combines BST search, insertion, and deletion (via `detach`) into one
operation.

## What is already implemented for you

The BST `insert` and `detach`-based delete code are provided in
`search-replace.cpp`:

* `tree::insert(int v)` — standard iterative BST insertion, modified to
  ignore duplicates: inserting a value already present leaves the tree
  unchanged.
* `tree::find(int key) const` — standard iterative BST search; returns the
  node holding `key`, or `nullptr`.
* `tree::where`, `tree::minimum`, `tree::detach`, `tree::erase` —
  detach-based deletion (`erase` calls `detach` then frees the node).

You should not modify any of these. Your only task is
`tree::searchReplace`.

## Constraints

- `0 <= N`, `1 <= Q` per test case; the total work across all test cases
  fits comfortably within the time limit for an `O(h)`-per-query solution.
- The `N` initial keys are distinct.
- `x` and `y` fit in a 32-bit signed integer, and may or may not be
  present in the tree at the time of a query.
- The tree never contains duplicate keys, before or after any query.

## Limits

| Function | Time | Auxiliary space |
|---|---|---|
| `searchReplace` | `O(h)`, where `h` is the current height of the tree | `O(1)` |

A solution that deletes `x` and re-inserts `y` on every call, without
checking whether `y` could have been written directly into `x`'s node, is
still `O(h)` — but it changes the tree's *shape* whenever `x`'s node has
two children, even though the final set of keys is the same either way.
The test cases check the tree's exact shape after every query, so such a
solution will fail some of them.

## Input Format

- Line 1: `T` — the number of independent test cases.
- For each test case:
  - A line containing `N`, the number of keys to insert.
  - A line containing `N` distinct integers — inserted in order to build
    the starting tree.
  - A line containing `Q`, the number of queries.
  - `Q` lines, each `x y`, applied in order via `searchReplace(x, y)`.
    Each query acts on the tree left behind by the previous one.

## Output Format

After every query, print one line containing the tree's current
level-order traversal (breadth-first, left to right within each level),
values separated by single spaces. If the tree is empty, print an empty
line.

### Example

#### Sample Input

```text
3
7
50 30 70 20 40 60 80
1
40 45
7
50 30 70 20 40 60 80
1
70 100
7
50 30 70 20 40 60 80
1
40 60
```

#### Sample Output

```text
50 30 70 20 45 60 80
50 30 80 20 40 60 100
50 30 70 20 60 80
```

#### Explanation

All three test cases start from the same tree:

```text
          50
        /    \
      30      70
     /  \    /  \
   20   40  60   80
```

**Test case 1**, `searchReplace(40, 45)`: node `40` is a leaf sitting
between `30` and `50` in sorted order; `45` fits between them too, so the
node's key is overwritten in place and the tree's shape is unchanged.

**Test case 2**, `searchReplace(70, 100)`: node `70` has two children
(`60` and `80`), so it must stay between `60` and `80` in sorted order;
`100` does not fit there, so `70` is deleted (via `detach`) and `100` is
inserted fresh, landing as the new right child of `80`.

**Test case 3**, `searchReplace(40, 60)`: `60` is already present
elsewhere in the tree, so `40` is simply deleted — `60` is not inserted
again.

## Hints

Read these only if you are stuck.

1. A value `y` can be written directly into the node currently holding
   `x` exactly when it falls strictly between that node's in-order
   predecessor and in-order successor in the *whole* tree (using "no
   bound" if one of them doesn't exist).
2. If the node has a left child, its predecessor is the maximum of that
   left subtree; otherwise, climb toward the root until you go up from a
   right child — that ancestor is the predecessor (or there is none, if
   you never do). The successor is the mirror image (`minimum` is already
   provided).
3. Check whether `y` is already present in the tree *before* checking
   whether it fits in `x`'s node — a duplicate must never be inserted, no
   matter what the fit check would say.

## Your Task

Write your code in `search-replace.cpp`, inside the body of:

```cpp
void tree::searchReplace(int x, int y) {
    // Your code starts from here -- DO NOT EDIT ANYTHING ABOVE
    ...
    // Your code ends here -- DO NOT EDIT ANYTHING BELOW
}
```

Do not delete or move the marker comments, do not write anything outside
them, and do not change any other file. You must not edit
`search-replace.h`, `common.h`, or `main.cpp`.

### Building and Running

```bash
make build      # compiles main.cpp + search-replace.cpp into ./search-replace
make run        # builds and runs it interactively (reads from stdin)
make runtests   # builds and runs all 8 test cases in tests/, comparing output
make clean      # removes compiled objects and the binary
```
