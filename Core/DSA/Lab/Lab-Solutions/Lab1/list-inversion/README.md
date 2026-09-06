# Lab 02b - Chromosomal Inversion (Linked List Edition)

[Ctrl + Shift + V to see the rendered version of this file]

## Background

DNA is a long sequence of bases, written with the letters `A`, `C`, `G` and `T`.
Sometimes a whole chromosome breaks off, flips around, and reattaches, so every
base ends up in the opposite order. Biologists call this an inversion.

You are writing the firmware for a newer revision of the bio-sensor. This revision's
memory controller cannot guarantee a contiguous block large enough to hold the whole
sequence as an array — bases are read in one at a time and stashed wherever a free
memory cell happens to be. So the sequence is stored as a singly linked list, one base
per node, each node pointing to the next. The sensor still has almost nothing left over
for scratch space.

## Problem Statement

You are given a DNA sequence of length `N`, stored as a singly linked list of `Node`s
(defined below).

Reverse the entire list in place, so the bases come out in the opposite order.
The list must remain correctly linked afterwards.

Reading the input, building the list, looping over the test cases, printing the answer
and freeing the memory are already done for you in `main.cpp`. All you have to write is
`reverse_list`.

## Your Task

Write your code in `list_inversion.cpp`, inside the body of `reverse_list`:

```cpp
Node* reverse_list(Node* head) {
    // Your code starts from here -- DO NOT EDIT ANYTHING ABOVE
    ...
    // Your code ends here -- DO NOT EDIT ANYTHING BELOW
}
```

Do not delete or move the two marker comments, do not write anything outside them, and
do not change any other file. Everything this problem needs fits inside the function
body, so you do not need helper functions here.

## Directory Structure

```
list-inversion.cpp    # write your solution here
list-inversion.h       # declaration of Node and reverse_list (DO NOT MODIFY)
common.h                # the headers you are allowed to use (DO NOT MODIFY)
main.cpp                 # reads input, builds/frees the list, calls your function, prints the result (DO NOT MODIFY)
README.md                # this file (DO NOT MODIFY)
Makefile                 # build automation
```

## Rules of the Problem

### Node Definition

Declared in `list_inversion.h`, already built for you by `main.cpp`:

```cpp
struct Node {
    char base;
    Node *next;
};
```

### Function Specification

#### `Node* reverse_list(Node* head)`

Declared in `list_inversion.h`, called by `main.cpp`, written by you in
`list_inversion.cpp`.

It reverses every node of the list, so the base that used to be last ends up first,
by relinking the existing nodes. It returns the head of the resulting list.

Parameters:

- `head` - pointer to the first node of the DNA sequence

Return value:

- The function must return a pointer to the new head of the list. Reversing the list
  always changes which node sits at the front (the node that used to be last is now
  first), so the caller cannot keep using the old `head` pointer — this is why the
  function returns a new one instead of taking `Node*&`.

A list of length 1 is already reversed, so the list must come out with the same single
node (though you must still return the correct head).

The function must not allocate any new nodes and must not delete any existing ones. It
only relinks the `next` pointers of the nodes it is given. `main.cpp` prints the list
by walking it from the returned head and frees every node afterwards, so a version
that builds a second list and returns that instead earns no marks.

### Constraints

| Resource | Limit |
|---|---|
| Time | `O(N)` per test case |
| Auxiliary space | `O(1)` |

- Allocating a second list, array, or any container that holds a copy of the sequence
  uses `O(N)` extra memory and is not allowed. The only nodes that may exist in memory
  are the `N` nodes you were handed.
- `reverse_list` may use only a constant number of extra variables — a handful of
  `Node*` pointers to walk and relink the list, and nothing that grows with `N`.
- Do not use recursion to reverse the list: each recursive call keeps a stack frame
  alive, which costs `O(N)` auxiliary space for a list of length `N`, breaking the
  `O(1)` space bound.
- Move through the list by reading and rewriting `->next` pointers directly. Do not use
  library helpers such as `std::reverse`, `std::list`, or `std::swap`, and do not
  include any extra headers. Everything you are allowed to use is already included by
  `common.h`.

### Input and Output

Input format:

```
Q
N_1
sequence_1
N_2
sequence_2
...
```

- `Q` is the number of independent test cases
- For each test case: the length `N`, then the `N` bases on their own line

The bases are read one at a time and linked into a list of `N` `Node`s, in order; that
part is already written for you.

Output format: one line per test case, holding the `N` bases with no separators after
the inversion, read off the list starting from the head your function returned.

Example input:

```
2
10
AACGTTGGCA
6
ACGTAC
```

Example output:

```
ACGGTTGCAA
CATGCA
```

Walkthrough of the first query:

```
position: 0 1 2 3 4 5 6 7 8 9
list:     A A C G T T G G C A
reversed: A C G G T T G C A A
          (the whole list is flipped end to end)
```

The second query reverses the whole six-base list; note that the returned head is a
different node than the one `main.cpp` originally passed in.

### Assumptions

- `1 <= N <= 10^7`
- every node's `base` is one of `A`, `C`, `G` and `T`

### Hint (only if you are stuck)

Walk the list once from the head, keeping three pointers: the node you are on, the node
just before it (starts out as "nothing", since the head has no predecessor), and the
rest of the list still ahead of you. At each step, save the next node, point the current
node's `next` back at its predecessor, then slide all three pointers forward by one.
When you run off the end of the list, the predecessor pointer is sitting on the old
last node, which is the head of the reversed list — return it. Drawing three or four
nodes on paper and flipping one arrow at a time before you touch any pointers will save
you from losing the rest of the list.

## Bonus

Once this works, think about how you would reverse only a chosen segment of the list —
positions `L` through `R` — instead of the whole thing, using only pointer relinking
and no new nodes. The `reverse_list` you just wrote is the special case `L = 0`,
`R = N - 1`, and it gives you all the pointer-surgery techniques that problem needs.
