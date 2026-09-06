# Lab 01 (Bonus) - Chromosomal Translocation

[Ctrl + Shift + V to see the rendered version of this file]

> This is an optional bonus problem. Solve `inversion/README.md` first. It is the
> problem for this lab, and the function you write there is the only tool you need
> here. Come back to this one once inversion works.

## Background

An inversion flips a segment of DNA in place, which is what you implemented in the
previous problem. A translocation is different. A segment of a chromosome breaks off
and reattaches at a completely different position.

You are working on the same memory-starved bio-sensor. It can hold the DNA sequence
itself, but it has almost no memory left over for scratch space, and it must finish
before the next reading arrives.

## Problem Statement

You are given a DNA sequence of length `N`, stored as a `vector<char>` named `v`, and a
translocation query made of three 0-indexed integers `L`, `R` and `P`.

The segment `v[L..R]`, both ends included, is cut out of the sequence and re-inserted
immediately before the base that was originally at index `P`.

- If `P > R`, the segment moves to the right, and the bases that were between `R` and
  `P` slide left to fill the gap.
- If `P < L`, the segment moves to the left, and the bases that were between `P` and
  `L` slide right.

Reading the input, looping over the test cases and printing the answer are already done
for you in `main.cpp`.

## Your Task

Write your code in `translocation.cpp`. It has two functions to fill in, each with its
own pair of markers:

```cpp
void reverse_segment(vector<char> &v, int L, int R) {
    // Your code starts from here -- DO NOT EDIT ANYTHING ABOVE
    ...
    // Your code ends here -- DO NOT EDIT ANYTHING BELOW
}

void apply_translocation(vector<char> &v, int L, int R, int P) {
    // Your code starts from here -- DO NOT EDIT ANYTHING ABOVE
    ...
    // Your code ends here -- DO NOT EDIT ANYTHING BELOW
}
```

Do not delete or move the marker comments, do not write anything outside them, and do
not change any other file.

## Directory Structure

```
translocation.cpp    # write your solution here
translocation.h      # declarations of the two functions (DO NOT MODIFY)
common.h             # the headers you are allowed to use (DO NOT MODIFY)
main.cpp             # reads input, calls your function, prints the result (DO NOT MODIFY)
README.md            # this file (DO NOT MODIFY)
Makefile             # build automation
```

## Rules of the Problem

### Function Specifications

Both functions are declared in `translocation.h` and written by you in
`translocation.cpp`. This folder is independent of the inversion folder, so start by
pasting in the body of the `reverse_segment` you wrote there.

#### `void reverse_segment(vector<char> &v, int L, int R)`

Reverses the elements of `v` from index `L` to index `R`, inclusive, in place. Exactly
the function from the inversion problem, unchanged. If `L >= R` there is nothing to do.

#### `void apply_translocation(vector<char> &v, int L, int R, int P)`

Performs the translocation described above on `v`, modifying `v` in place. This is the
one `main.cpp` calls, and it may call `reverse_segment` as often as it likes.

Parameters:

- `v` - the DNA sequence, modified in place
- `L`, `R` - first and last index of the segment that breaks off, `L <= R`
- `P` - the segment is re-inserted immediately before the base originally at index
  `P`; `P` never lies strictly inside the segment, i.e. `L < P <= R` never occurs

Note that `P == L` and `P == R + 1` both mean "put the segment back exactly where it
was", so the sequence must come out unchanged.

### Constraints

A solution that ignores these will fail on the large tests.

| Resource | Limit |
|---|---|
| Time | `O(N)` per test case |
| Auxiliary space | `O(1)` |

- Sliding the segment one position at a time costs a full pass over the segment for
  every position it travels, and it can travel `O(N)` positions. That is `O(N^2)` work
  in total, which is too slow here.
- Copying the segment into a temporary `vector` (or array, or `string`), rewriting the
  original, and pasting the copy back is `O(N)` extra memory and is not allowed.
- `apply_translocation` may use only a constant number of extra variables, say indices
  and a single `char` for swapping. The only large object in memory is `v` itself.
- Move the bases around by writing to `v[i]` directly. Do not use library helpers such
  as `std::reverse`, `std::rotate`, `v.insert()` or `v.erase()`, and do not include any
  extra headers. Everything you are allowed to use is already included by `common.h`.

### Input and Output

Input format:

```
Q
N_1
S_1
L_1 R_1 P_1
N_2
S_2
L_2 R_2 P_2
...
```

- `Q` is the number of independent test cases
- For each test case: the length `N`, then the `N` bases on their own line, then the
  three query integers `L`, `R` and `P` on the next line

The bases are read one at a time into a `vector<char>` of size `N`; that part is
already written for you.

Output format: one line per test case, holding the `N` bases with no separators after
the translocation.

Example input:

```
2
10
AAAGGGCCCT
0 2 6
10
GGGAAACCCT
3 5 0
```

Example output:

```
GGGAAACCCT
AAAGGGCCCT
```

Walkthrough of the first query (`L = 0`, `R = 2`, `P = 6`):

```
index:    0 1 2 3 4 5 6 7 8 9
v:        A A A G G G C C C T
segment:  ^^^^^ (v[0..2] = A A A)
insert immediately before index 6 (the first 'C')

cut:      _ _ _ G G G C C C T
result:   G G G A A A C C C T
```

The second query moves the same segment back again.

### Assumptions

- `1 <= N <= 10^7`, and the total length over all test cases also fits in memory
- `0 <= L <= R <= N - 1`
- `0 <= P <= N`, and `P` is guaranteed not to fall strictly inside the segment:
  `L < P <= R` never occurs. `P == L` and `P == R + 1` are the two queries that change
  nothing, and `P == N` means the segment is appended at the very end
- every element of `v` is one of the bases `A`, `C`, `G` and `T`

### Hint (only if you are stuck)

Reversing does not look like it has anything to do with moving, so here is where to
start.

Look at the part of the sequence that actually changes. Everything before it and
everything after it stays put. Write that window down as two blocks, and compare it
with what the window has to look like afterwards. The two blocks have simply traded
places.

So: how do you swap two adjacent blocks of different lengths, with nowhere to put
anything down? You can call `reverse_segment` as many times as you like, on any part of
the sequence. Try it by hand on a small example, on paper, before you code.

---
