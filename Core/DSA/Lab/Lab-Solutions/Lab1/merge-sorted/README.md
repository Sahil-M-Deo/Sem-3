# Lab02 - Merge Sorted Linked Lists

## Problem Statement

You are given two singly linked lists, `L1` and `L2`, each represented by a
`list` struct (tracked via `head`/`tail` pointers to `node`s). Each list is
already sorted in **non-decreasing** order of `val`. Merge them into a
**single** new list that is also sorted in non-decreasing order, and return
a pointer to it.

`empty`, `detach_front`, and `attach_back` are already implemented for you
in `merge-sorted.h` -- you do not need to (and should not) reimplement
them. Use them to move nodes between lists.

You must build the merged list **in place**, by moving the existing nodes
of `L1` and `L2` (e.g. via `detach_front`/`attach_back`) into a new `list`.
Do **not** allocate any new `node`s, and do **not** copy the values into an
array or `std::vector` and rebuild the list from that. You may allocate one
new `list` to hold the result.

You must implement exactly one function:
- `list* merge(list* L1, list* L2)` -- merges `L1` and `L2` and returns a pointer to the merged, sorted list.

---

## Constraints

Let `L1` hold values $a_1 \le a_2 \le \dots \le a_{n_1}$ and `L2` hold values
$b_1 \le b_2 \le \dots \le b_{n_2}$.

- $0 \le n_1, n_2 \le 10^5$
- $-10^9 \le a_i, b_i \le 10^9$

($n_1 = 0$ and/or $n_2 = 0$ means that list is empty, i.e. `L1->empty()`/`L2->empty()` is `true`.)

## **Rules of the Problem**

- Given the above, `merge(L1, L2)` must return a list holding values $c_1, c_2, \dots, c_{n_1+n_2}$ such that:
  - $c_1 \le c_2 \le \dots \le c_{n_1 + n_2}$ (the output is sorted), and
  - $\{c_1, \dots, c_{n_1+n_2}\} = \{a_1, \dots, a_{n_1}\} \uplus \{b_1, \dots, b_{n_2}\}$ (multiset union -- every value from `L1` and every value from `L2` appears in the output exactly as many times as it appeared in the input, no more, no less).
- You must move the existing nodes (e.g. with `detach_front`/`attach_back`) rather than allocating new `node`s, and must not change the `val` field of any node.
- Do not print anything from inside `merge` -- all printing is handled for you in `main.cpp`.
- Do not modify `main.cpp`, `common.h`, or anything in `merge-sorted.h`.

---

### Example:

### **Sample Input**
```
3
1 3 5
3
2 4 6
```

### **Sample Output**
```
1 2 3 4 5 6
```

### **Explanation**

- `L1` has `3` values: `1 3 5`.
- `L2` has `3` values: `2 4 6`.
- Merging them, smallest value first, gives `1 2 3 4 5 6`.

---

### Another Example (an empty list):

### **Sample Input**
```
0

4
2 2 5 9
```

### **Sample Output**
```
2 2 5 9
```

### **Explanation**

- `L1` is empty (`0` values, so the second line is blank).
- `L2` has `4` values: `2 2 5 9` (note the repeated `2` -- both copies must appear in the output).
- Since `L1` is empty, the merged list is simply `L2`.

---

## Input Format

- Line 1: $n_1$
- Line 2: $a_1 \ a_2 \ \dots \ a_{n_1}$ (blank if $n_1 = 0$)
- Line 3: $n_2$
- Line 4: $b_1 \ b_2 \ \dots \ b_{n_2}$ (blank if $n_2 = 0$)

## Output Format

- Print a single line containing the values of the merged list, in non-decreasing order, separated by a single space. If the merged list is empty, print just a newline.

---

## Your Task

You must write your implementation in the following file.
```
merge-sorted.cpp
```

Please edit the code only in the marked area (between `Your code starts from here` and `Your code ends here`) and do not edit anywhere else in this file. You must not edit `merge-sorted.h`, `common.h`, or `main.cpp`.

---

## Directory Structure

```
merge-sorted.cpp    # implement your solution here
merge-sorted.h       # node/list struct definitions + function declaration (DO NOT MODIFY)
common.h             # shared includes (DO NOT MODIFY)
main.cpp             # reads input, builds the two lists, calls merge, prints the result (DO NOT MODIFY)
README.md            # This file (DO NOT MODIFY)
Makefile             # Build automation
```

---
