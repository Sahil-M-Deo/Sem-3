# Lab 02 - Doubly Linked List with a Sentinel

[Ctrl + Shift + V to see the rendered version of this file]

## Problem Overview

Complete the doubly linked list from the lecture slide. It uses a **sentinel**
node: `sentinel.next` is the head, `sentinel.prev` is the tail, an empty list is
the sentinel pointing at itself, and there is no `nullptr` anywhere. `end()` is
the sentinel itself, so `insert` and `erase` need no special cases for the ends.

The class keeps two extra members: `sz`, the element count, and `flipped`, a
reversal flag.


Suppose the list contains

10 <-> 20 <-> 30

Then the pointers are


    +-----------+     +----+     +----+     +----+
    | sentinel  |<--->| 10 |<--->| 20 |<--->| 30 |
    +-----------+     +----+     +----+     +----+
         ^                                   |
         |                                   |
         +-----------------------------------+

## Node and Class Structure

```cpp
struct node { int val; node *next, *prev; };

class list {
    node sentinel;   // sentinel.next is head, sentinel.prev is tail
    int  sz;         // number of elements
    bool flipped;    // true => next and prev are read the opposite way
public:
    list();                                // given
    node* end();                           // given
    void  insert_before(node* p, int v);   // given: insert v before p
    void  erase(node* p);                  // given: unlink and delete p
    ...                                    // yours (see Your Task)
};
```

## The Flag

`reverse()` must not move any node or rewrite any pointer. It only toggles
`flipped`, and every other function reads the list accordingly:

| `flipped` | successor of `p` | predecessor of `p` | first element |
|---|---|---|---|
| `false` | `p->next` | `p->prev` | `sentinel.next` |
| `true`  | `p->prev` | `p->next` | `sentinel.prev` |

`insert_before` and `erase` are already written for you. Read them first: they
show the convention your code must follow.

## Your Task

Implement these nine members. `begin`, `next`, `prev`, `size` and `reverse` must
be `O(1)`.

```cpp
~list();                  // free every node (not the sentinel)
node* begin();            // first element, or end() if empty
node* next(node* p);      // logical successor; end() after the last element
node* prev(node* p);      // logical predecessor; prev(end()) is the last element
int   size() const;       // return the counter, do not walk the list
void  reverse();          // toggle the flag
void  join(list& other);  // append other, then leave other empty
void  print();            // values in order, space separated, then a newline
node* find(int v);        // first node holding v, else end()
```

- `find` returns `end()` when the value is absent, like `std::list`.
- `L1.join(L2)` leaves `L1` holding its own elements followed by `L2`'s, and
-  L1.join(L2) leaves L1 holding its own elements followed by L2's, and L2 a valid empty list. Handle an empty side, the two flags disagreeing, and L.join(L) (do nothing). Keep sz correct on both.

- `main.cpp` frees nothing, so `~list()` must free every node.

Do not change any signature, add members, or edit any other file.

## Input Format

Each test case drives **two** lists, L`0` and L`1`, so `join` can be tested. Every
command names its list. Positions are 0-indexed.

```
push_front L V          push_back L V           insert_before L P V
erase_at L P            reverse L               join L1 L2
find L V                size L                  print L            rprint L
```

The last line functions print one line each: the index of the first `V` (or `-1`), the
size, the values from `begin()`, and the values walked backwards with `prev()`.

Number of testcases in 1 file is `Q`, then for each test case a command count `M` and the `M` commands.

### **Sample Input**

```
2                  <- Q: two test cases follow
11                 <- case 1 has 11 commands
push_back 0 1
push_back 0 2      L0 = 1 2
push_back 1 3
push_back 1 4      L1 = 3 4
print 0
print 1
join 0 1           L0 absorbs L1
print 0
size 0
print 1
size 1
8                  <- case 2 has 8 commands, both lists start fresh again
push_back 0 1
push_back 0 2      L0 = 1 2
push_back 1 3
push_back 1 4      L1 = 3 4
reverse 1          L1 now reads 4 3
print 1
join 0 1
print 0
```

### **Sample Output**

output for print ,reverse and rprint: 
```
1 2                case 1: L0 before
3 4                case 1: L1 before
1 2 3 4            after join
4                  size of L0
                   L1 is now empty, so print emits a blank line
0                  size of L1
4 3                case 2: L1 after reverse
1 2 4 3            after join, L1's elements arrive in its logical order
```

## Your Task

You must write the implementation in the following file.

```
doubly-linked-list.cpp
```

Please edit the code in the given area and do not edit anywhere else.

## Directory Structure

```
doubly-linked-list.cpp  # implement your solution here
doubly-linked-list.h    # node and list declarations (DO NOT MODIFY)
common.h                # headers you may use (DO NOT MODIFY)
main.cpp                # driver (DO NOT MODIFY)
README.md               # This file (DO NOT MODIFY)
Makefile                # Build automation
```
