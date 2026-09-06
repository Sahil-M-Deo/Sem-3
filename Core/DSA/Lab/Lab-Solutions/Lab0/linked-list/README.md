# Lab01 - Linked List


## Problem Statement

Implement a **sorted singly linked list** of integers using an actual linked
list of nodes (i.e. `struct Node` objects connected via pointers)

You should not use an
array or `std::vector`

Numbers are given to your program one at a time. Each time a number arrives,
insert it into the linked list at the correct position so that the list
stays sorted in **non-decreasing** order, then print the current contents of
the list. The list is printed once after every single insertion to check that it stays sorted at every step.

You must implement 4 member functions of the `LinkedList` class:
- `LinkedList::LinkedList()` -- the constructor.
- `LinkedList::~LinkedList()` -- the destructor. Free every node you allocated, to avoid memory leaks.
- `void LinkedList::insertSorted(int value)` -- inserts `value` into the list, keeping it sorted.
- `void LinkedList::print() const` -- prints the list's values in order, separated by a single space, followed by a newline.

---

## **Rules of the Problem**

- `insertSorted` must insert `value` at the correct position so the list remains sorted in non-decreasing order (smallest to largest).
- Duplicate values are allowed and must all be kept, e.g. inserting `5, 5, 3` should give the list `3, 5, 5`.
- `print` must print all values currently in the list, in order, space-separated, followed by a newline (`endl`/`"\n"`). If the list is empty, just print a newline.
- Your `insertSorted` and `print` should work correctly for a list of any length, including an empty list.
- Your destructor must free all dynamically allocated nodes (no memory leaks). It's fine to rely on the destructor being called only once, at the end of the program.
- Do not print anything from inside `insertSorted` -- all printing happens in `print`, which is already called for you after every insertion.
- Do not modify `main.cpp`, `common.h`, or the class/function signatures in `linked-list.h`.

---

### Example:

### **Sample Input**
```
5
5 3 8 1 9
```

### **Sample Output**
```
5
3 5
3 5 8
1 3 5 8
1 3 5 8 9
```

### **Explanation**

- Insert `5` -> list is `5` -> print `5`
- Insert `3` -> list is `3 5` -> print `3 5`
- Insert `8` -> list is `3 5 8` -> print `3 5 8`
- Insert `1` -> list is `1 3 5 8` -> print `1 3 5 8`
- Insert `9` -> list is `1 3 5 8 9` -> print `1 3 5 8 9`

---

## Input Format

- The first line contains a single integer `n`, the number of values to insert.
- The second line contains `n` space-separated integers, given in the order they should be inserted.

## Output Format

- After each of the `n` insertions, print the entire current list (space-separated, one line per insertion). There should be exactly `n` lines of output.

---

## Your Task

You must write your implementation in the following file.
```
linked-list.cpp
```

Please edit the code only in the marked area (between `Your code starts from here` and `Your code ends here`) and do not edit anywhere else in this file. You must not edit `linked-list.h`, `common.h`, or `main.cpp`.

---

## Directory Structure

```
linked-list.cpp    # implement your solution here
linked-list.h      # class/struct declarations (DO NOT MODIFY)
common.h           # shared includes (DO NOT MODIFY)
main.cpp           # reads input, drives insertion + printing (DO NOT MODIFY)
README.md          # This file (DO NOT MODIFY)
Makefile           # Build automation
```

---
