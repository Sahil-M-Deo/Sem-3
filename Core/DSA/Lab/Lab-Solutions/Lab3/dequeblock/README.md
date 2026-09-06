# Lab04 - Deque using dequeBlocks

## Problem Statement

This lab has **two parts**, following the construction sketched in
**Lecture 08** (Deques and Amortisation).

### Part 1: `dequeBlock<T>`

Implement `dequeBlock<T>` — a **fixed-capacity** deque implemented using a
**circular array**. It supports push/pop from either end and random access,
all in O(1) time. Once the block is full, push operations must **fail**
(return `false`) instead of growing — `dequeBlock` never reallocates itself.

### Part 2: `Deque<T>`

Using `dequeBlock<T>` from Part 1, implement `Deque<T>` — a full,
**dynamically-growing** deque built as a **"directory" of dequeBlocks**,
exactly as sketched in the lecture:

- Actual data is stored across many fixed-capacity `dequeBlock<T>` objects.
- A **directory** — itself a `dequeBlock<dequeBlock<T>*>` — holds pointers to
  these data blocks, so it can be indexed as `directory[d]` to get the `d`-th
  data block.
- When the directory itself becomes full, it is **reallocated** (like
  `std::vector`'s doubling strategy): a bigger `dequeBlock` is allocated, the
  existing pointers are copied across, and the old directory is discarded.
- New data blocks are allocated lazily — only when the current front/back
  block is full — and freed once they become empty.

`dequeBlock` is deliberately used **both** to hold actual data (`T`) **and**
to hold pointers to data blocks (`dequeBlock<T>*`) inside the directory —
that's why it's written as a template.

## Approach

**Part 1 (`dequeBlock<T>`):** Keep a fixed-size array `data`, a `startIdx`
marking the current front, and a `count` of stored elements. `pushBack`
writes to `(startIdx + count) % cap`; `pushFront` decrements `startIdx`
(wrapping around) and writes there. Popping is the mirror image. Random
access `operator[](i)` reads `data[(startIdx + i) % cap]`.

**Part 2 (`Deque<T>`):** Maintain the invariant that **every data block
except possibly the front-most and rear-most is completely full**. This is
what makes O(1) random access possible: if `k` is the number of elements in
the front-most block and `N` is the fixed block capacity, then index `i`
lives in block

```
d = 0,                  j = i        if i < k
d = (i - k) / N + 1,    j = (i - k) % N   otherwise
```

`pushBack`/`pushFront` allocate a fresh block (growing the directory first,
if needed) only when the current back/front block is already full —
this preserves the invariant. `popBack`/`popFront` remove the emptied
extreme block from the directory once it hits zero elements — this also
preserves the invariant, since the block being promoted to the new
front/back was already full (or is the sole remaining block).

## Input Format

- Line 1: `Q` — the number of commands.
- Each of the next `Q` lines is one command (see below).

### `dequeBlock<int>` commands (Part 1)
```
CREATE_BLOCK cap        # (re)creates the block with the given fixed capacity
PUSH_BACK_BLOCK x        # prints 1 on success, 0 if the block is full
PUSH_FRONT_BLOCK x       # prints 1 on success, 0 if the block is full
POP_BACK_BLOCK           # prints the popped value, or -1 if empty
POP_FRONT_BLOCK          # prints the popped value, or -1 if empty
FRONT_BLOCK               # prints the front value, or -1 if empty
BACK_BLOCK                # prints the back value, or -1 if empty
AT_BLOCK i                 # prints the value at index i (0-indexed from front)
SIZE_BLOCK                 # prints the current number of elements
IS_FULL_BLOCK              # prints 1 if full, 0 otherwise
IS_EMPTY_BLOCK             # prints 1 if empty, 0 otherwise
```

### `Deque<int>` commands (Part 2)
```
CREATE_DEQUE blockCap    # (re)creates the deque; blockCap = capacity of each internal dequeBlock
PUSH_BACK x
PUSH_FRONT x
POP_BACK                  # prints the popped value, or -1 if empty
POP_FRONT                 # prints the popped value, or -1 if empty
GET_FRONT                 # prints the front value, or -1 if empty
GET_BACK                  # prints the back value, or -1 if empty
AT i                       # prints the value at index i (0-indexed from front)
SIZE                       # prints the current number of elements
IS_EMPTY                   # prints 1 if empty, 0 otherwise
```

`CREATE_BLOCK` / `CREATE_DEQUE` and `PUSH_*` (on `Deque`, not `dequeBlock`)
produce no output. Always issue `CREATE_BLOCK` before any `*_BLOCK` command,
and `CREATE_DEQUE` before any `Deque` command, in a given test case.

## Output Format

For every command that produces a value (see above), print it on its own
line.

## Example

### Sample Input
```
7
CREATE_DEQUE 2
PUSH_BACK 1
PUSH_BACK 2
PUSH_BACK 3
POP_FRONT
GET_FRONT
SIZE
```

### Sample Output
```
1
2
2
```

## Explanation

`blockCap = 2`, so the first `dequeBlock` (capacity 2) fills up after
`PUSH_BACK 1` and `PUSH_BACK 2`. `PUSH_BACK 3` allocates a **second**
`dequeBlock` and stores `3` there. `POP_FRONT` removes `1` from the
front-most block (now down to just `[2]`). `GET_FRONT` reads `2`. `SIZE`
reports `2` elements remaining (`2` and `3`, across two blocks).

## Hints

Read these only if you are stuck.

1. `dequeBlock::operator[]` and `dequeBlock::front()`/`back()` don't need to
   check bounds — the caller (your `Deque<T>` code, or the test driver)
   guarantees the index/call is valid.
2. In `Deque<T>::pushBack`, check whether the directory is empty **or** its
   back block is full **before** deciding whether to allocate a new block —
   both cases mean "there's no room to push into the current back block".
3. When growing the directory, copy the pointers in **front-to-back order**
   using the old directory's `operator[]`, so the new directory's element
   order matches the old one exactly.
4. In `Deque<T>::operator[]`, `k = directory->front()->size()` is the only
   block whose size you need to know explicitly — every other block from
   index `1` onward is guaranteed to be full (capacity `N`), so plain
   division/modulo locates the right block.
5. When a block becomes empty after a pop, don't forget to `delete` it after
   removing its pointer from the directory — otherwise you'll leak memory.

## Your Task

You must write **both parts** in a single file:

```
dequeblock.cpp   # Part 1 (dequeBlock<T>) AND Part 2 (Deque<T>)
```

Inside that file the two parts are separated by comment banners
(`PART 1: dequeBlock<T>` and `PART 2: Deque<T>`). Every member function is
already given to you as an empty stub with its own marked block:

```cpp
template <typename T>
bool dequeBlock<T>::pushBack(const T& val) {
    // Your code starts from here -- DO NOT EDIT ANYTHING ABOVE



    // Your code ends here -- DO NOT EDIT ANYTHING BELOW
}
```

Fill in the body of each function **inside** its marked block. Do not change
the function signatures, the banners, the `#include`s at the top of the file,
or anything else outside the marked blocks. You must not edit `dequeblock.h`,
`deque.h`, `common.h`, or `main.cpp`.

Note that the constructors initialise their members by assignment inside the
body (not with an initialiser list), so that all of your code stays within the
marked block.

## Constraints

- `1 <= Q <= 10^4` (number of commands per test case)
- `-10^9 <= x <= 10^9` (values pushed)
- `1 <= cap, blockCap <= 10^3`
- All `dequeBlock` operations (push/pop/front/back/random access) must run
  in O(1) time.
- All `Deque` push/pop operations must run in O(1) amortised time; random
  access (`operator[]`) must run in O(1) worst-case time.

## Limits

| Function                  | Time (amortised) | Auxiliary Space |
|----------------------------|-------------------|------------------|
| `dequeBlock::pushBack/Front` | O(1)             | O(1)             |
| `dequeBlock::popBack/Front`  | O(1)             | O(1)             |
| `dequeBlock::operator[]`      | O(1)             | O(1)             |
| `Deque::pushBack/Front`        | O(1)             | O(1)             |
| `Deque::popBack/Front`         | O(1)             | O(1)             |
| `Deque::operator[]`             | O(1) worst-case  | O(1)             |

## Directory Structure

```
dequeblock.h    # Part 1 class declaration (do not edit)
deque.h          # Part 2 class declaration (do not edit)
dequeblock.cpp   # Part 1 + Part 2 implementations (YOUR TASK — edit only the marked section)
common.h          # shared includes (do not edit)
main.cpp          # reads commands, drives both dequeBlock<int> and Deque<int> (do not edit)
README.md          # this file
tests/              # 12 test cases, one folder per test (input.txt + output.txt)
Makefile            # build / run / runtests / clean automation
```

### Building and Running

Both classes are templates, so there is nothing to compile separately:
`dequeblock.cpp` is pulled in through the headers and compiled as part of
`main.cpp`.

```bash
make build      # compiles main.cpp (with your dequeblock.cpp) into ./deque-using-dequeblock
make run        # builds and runs it interactively (reads from stdin)
make runtests   # builds and runs all 12 test cases in tests/, comparing output
make clean      # removes compiled objects and the binary
```


