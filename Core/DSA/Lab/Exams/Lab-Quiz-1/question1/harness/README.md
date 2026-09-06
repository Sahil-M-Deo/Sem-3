# Circular Doubly-Linked List with Sentinel Node

## Objective
Your task is to implement the **`merge`** and **`split`** member functions of the `list` structure inside **`circular.cpp`**.

---

## Overview
A circular doubly-linked list uses a **sentinel node** to simplify boundary management.

* The sentinel node acts as a fixed boundary element (`sentinel.next` points to the head, and `sentinel.prev` points to the tail).
* An empty list is represented by `sentinel.next == &sentinel` and `sentinel.prev == &sentinel`.
* Every valid data node maintains valid `next` and `prev` pointers, eliminating `nullptr` checks during link manipulations.

## File Structure

| File Name | Description |
| :--- | :--- |
| `circular.h` | Class declarations, node structure, constructors, move semantics, and display helpers |
| `main.cpp` | Driver program, I/O parsing, test execution, and memory cleanup |
| `circular.cpp` | **Implementation file where you will write `merge` and `split`** |

---

## Task Requirements

You must implement the following two member functions in **`circular.cpp`**:

### 1. `void list::merge(list& other)`
* Appends all nodes from `other` onto the end of `this` list in $O(1)$ constant time.
* Resets `other` to an empty list state (`other.sentinel.next == &other.sentinel`).
* Must correctly handle cases where either `this` or `other` is empty.

### 2. `list list::split(node* p)`
* Splits `this` list into two starting at pointer `p`.
* Nodes from `p` up to the end of `this` list are detached and moved into a new `list` object.
* Returns the newly created `list` object containing nodes starting from `p`.
* If `p` is `nullptr` or points to `&this->sentinel`, returns an empty list without modifying `this`.

## Example Test Case

This example shows a `merge` followed by a `split`.

**Input:**

```text
1 2 3 4
5 6 7
3
```

The first line represents the first list and the second line represents `other`. The final value `3` is the node pointer position at which the split is performed.

### Step-by-step

After `merge`, all nodes from `other` are appended to the first list:

![alt text](../../../../../../../Downloads/Circular%20Linked%20List/image.png)

`other` is reset to an empty circular list:



The split point is the node containing `3`. The nodes from `3` through the end are detached and returned as the new list:

![alt text](../../../../../../../Downloads/Circular%20Linked%20List/image-1.png)

**Expected output :**

```text
1 2 3 4 5 6 7

1 2
3 4 5 6 7
```

The first line is the merged list. After splitting at `3`, the original list contains `1, 2`, while the returned list contains `3, 4, 5, 6, 7`.
