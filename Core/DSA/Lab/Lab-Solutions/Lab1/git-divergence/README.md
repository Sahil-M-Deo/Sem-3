# Lab 02 - Git Branch Divergence

[Ctrl + Shift + V to see the rendered version of this file]

## Background

Git stores a project's history as a chain of commits. Each commit remembers the
commit it was built on top of, which is called its parent.

Alice branched `feature-A` off `main` and started committing on it. A few days
later Bob branched `feature-B` off a later commit of `main` and did the same at
his own pace. The two branches now have different lengths, but both grew out of
`main`, so if you follow either one back far enough you reach a commit that
belongs to both. Your job is to find that commit, the one where they split.

## The Technical Mapping

The history is a singly linked list of `Node`s. One `Node` is one commit, and
`next` points to the parent, so following `next` walks backwards in time. The
project's first commit is the only one whose `next` is `nullptr`.

A commit has exactly one parent here, so once the two branches reach a commit
they both have, they walk over the same nodes the rest of the way. The two lists
share a tail, which makes the picture a Y. Each branch's HEAD is on the left:

```
feature-A: [a1] -> [a2] -> [a3] --.
                                   \
                                    [s1] -> [s2] -> [s3] -> nullptr
                                   /
feature-B:         [b1] -> [b2] --'
```

Here `s1` is the answer: the newest commit that both branches can reach.

## Problem Statement

Given `headA` and `headB`, the HEADs of the two branches, return the commit
where they split, or `nullptr` if they have no commit in common.

Compare pointers, `a == b`. Two different commits are allowed to hold the same
`id`, so `a->id == b->id` finds the wrong commit and fails the tests.

Write two functions. Both must use `O(1)` extra space.

### Part A: The Exhaustive Search (naive)

```cpp
Node* findDivergenceNaive(Node* headA, Node* headB);
```

`O(m * n)` time, where `m` and `n` are the branch lengths. This part must be the
brute-force version: for each commit on one branch, search the whole of the
other. Anything faster belongs in Part B, not here.

### Part B: The Synchronized Rollback (optimized)

```cpp
Node* findDivergenceOptimized(Node* headA, Node* headB);
```

The same answer, in `O(m + n)` time.

`main.cpp` calls both functions on every test case and prints both answers, so
you need both parts working before a test will pass.

## Your Task

Write your code in `git-divergence.cpp`, between the two marker comments. Do not
move them, do not write outside them, and do not change any other file. Reading
the input, building the lists, printing the answers and freeing the memory are
already done for you in `main.cpp`.

## Directory Structure

```
git-divergence.cpp    # write your solution here
git-divergence.h      # declaration of Node and the two functions (DO NOT MODIFY)
common.h              # the headers you are allowed to use (DO NOT MODIFY)
main.cpp              # reads input, builds/frees the lists, calls your functions, prints (DO NOT MODIFY)
README.md             # this file (DO NOT MODIFY)
Makefile              # build automation
```

## Rules of the Problem

### Node Definition

Declared in `git-divergence.h`, already built for you by `main.cpp`:

```cpp
struct Node {
    int id;      // this commit's id
    Node* next;  // the parent commit (nullptr at the very first commit)
};
```

### Function Specification

Both functions take the two HEADs and return the divergence commit, the first
node reachable from `headA` that is also reachable from `headB`. Return
`nullptr` when there is none, which covers `headA` or `headB` being `nullptr`.
One branch can also sit entirely inside the other: if Alice branched off and
never committed, `headA` is itself the answer.

Neither function may:

- create a node, or `delete` one;
- write to `next` or `id`. Both run on the same lists, one after the other, so
  the second sees whatever the first left behind;
- copy a branch into an array, a `vector` or a `set`, which costs `O(m)` space;
- recurse over a branch, since every stack frame costs `O(m)` space too;
- print anything. `main.cpp` does all the printing.

Do not include any extra headers. `common.h` already has everything you need.

### Constraints

| Resource | Part A | Part B |
|---|---|---|
| Time | `O(m * n)` | `O(m + n)` |
| Auxiliary space | `O(1)` | `O(1)` |

- `1 <= Q <= 20`, the number of test cases in one input file
- `0 <= m_only, n_only, c <= 3000`, the commits exclusive to each branch and the
  length of the shared history, so `m = m_only + c` and `n = n_only + c`
- the sum of `m * n` over all `Q` test cases is at most `1.5 * 10^7`, so Part A's
  `O(m * n)` solution still finishes in time
- `-10^9 <= id <= 10^9`, and ids can repeat: the same id may sit on both branches
  without those two commits being the same commit

### Input and Output

```
Q
m_only_1 n_only_1 c_1
<c_1 ids of the shared history, newest first>
<m_only_1 ids exclusive to feature-A, newest first>
<n_only_1 ids exclusive to feature-B, newest first>
...
```

Each test case gives three counts, then three groups of ids in that order: the
shared history (first id read is the divergence commit, last is the project's
first commit), then feature-A's own commits starting at its HEAD, then
feature-B's the same way. A group whose count is `0` leaves a blank line, so
`c = 0` means the branches share nothing, and `m_only = 0` means `headA` is
itself the divergence commit.

Print one line per test case with two answers separated by one space,
`findDivergenceNaive` first. Each answer is the `id` of the commit returned, or
`NONE` for `nullptr`.

### Example

Input:

```
1
3 2 3
101 102 103
7 8 9
4 5
```

Output:

```
101 101
```

The shared history is `101 102 103`, so `101` is the divergence commit.
`feature-A` reads newest first as `7 8 9`, giving the full history
`9, 8, 7, 101, 102, 103`, and `feature-B`'s is `5, 4, 101, 102, 103`. The newest
commit on both is `101`, and both functions return that node.

## Points to Ponder

- Part A allocates nothing and calls no library function, yet it is far slower
  than Part B. Where does the time go?
- Part A can stop the moment it finds the answer. Is there an input where that
  makes it do less work than Part B?
- Real Git names a commit by a hash of its content and its history. Does that
  make comparing ids safe there? What about two commits with the same message
  and changes, made by two people at different times?
