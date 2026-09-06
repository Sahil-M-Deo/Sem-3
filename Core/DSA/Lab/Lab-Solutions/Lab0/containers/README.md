# Problem: Drift Runner is Back

DSA TAs have to deal with large number of files. A compromised `drift_runner` started to corrupt some of those files. 

The TAs, naturally, kept a log the entire time: every file
`drift_runner` ever touched. They
want to finally put that log file to use  a lookup service where, given
a file's ID, you can instantly tell whether `drift_runner` ever
claimed it, while new entries keep getting appended as old logs are
still being processed.

Two things happen constantly for this service to work:

- **Inserting** a new file ID into the log, as records get processed.
- **Finding** a specific file ID, the instant someone asks "wait, was this one of drift_runner's ?"

Before picking a container to actually store this log in, the TAs wanted data. So you are "the chosen one" to help them in this lab by benchmarking `vector`,
`list`, `set`, and `unordered_set` on exactly these two operations,
across a *range* of sizes, so you can see whether an operation scales
as O(1), O(log n), or O(n) as the number of records grows, instead of
just comparing them at one fixed size.

There's a second twist. `set` and `unordered_set` can both be
searched two different ways:

- The generic `std::find(begin, end, value)` algorithm, which just
  walks the container element by element - it works on *any*
  container, but has no idea it's looking at a tree or a hash table.\
  What's a tree and what's a hash table now ?!!\
  You will find soon enuf in the course :)

- Their own member function, `.find(value)`, which *does* know about
  the container's internal structure, and can use it to search much
  faster.

You will measure both, on the very same container, so you can see
exactly how much performance you'd leave on the table by using the
wrong search method.

# Timing your code

We will use the C++ `<chrono>` library to measure how long a block of
code takes to run. The pattern looks like this:

```cpp
#include <chrono>
...
auto start_time = std::chrono::steady_clock::now();
// code to be timed
auto duration = std::chrono::steady_clock::now() - start_time;
long long time_taken =
    std::chrono::duration_cast<std::chrono::microseconds>(duration).count();
```

`time_taken` now holds the elapsed time in microseconds. You will use
exactly this pattern, several times over, in `containers.cpp`.

# Your Task

Open `containers.cpp`. It declares four functions, one per container,
one of those is already scaffolded with the timing boilerplate and clearly
marked `TODO` sections where you can fill in the actual container operations.
Other three are left for you to implement on your own.

For every size `n` in a list of sizes, each function should:

1. Build a fresh, empty container.
2. Insert `n` elements, timing the loop using the `<chrono>` pattern
   above.
3. Look up all `n` elements, timing that too.
4. Repeat steps 1-3 `repeat` times for that size and average, storing
   the result in `results[n]`.

# Input/Output

Input (stdin):
Three non-negative numbers:
- `n_max` -- the largest container size to test.
- `step` -- the gap between successive sizes (so sizes tested are
  `step, 2*step, 3*step, ..., n_max`).
- `repeat` -- how many times to repeat the experiment at each size,
  for averaging.

Output (stdout):
One line per (container, size):

```
<name> <size> <insert_us> <find_std_us> <find_member_us>
```

`find_member_us` is `-1` for `vector` and `list`, since they have no
member `find()`.

# Visualizing the results

`make` / `make runtests` only build and run your code.
Once you're ready to *see* the curves, run the plotter:

```
make plot                 # plots the output of tests/test1
make plot TEST=test2      # plots the output of tests/test2
make plot TEST=test3      # plots the output of tests/test3
```

This saves an image, `plot_data_plot.png`, with three graphs side by
side. \
The three graphs are:
- **Insertion time vs. n**, one line per container.
- **Find time vs. n**, using `std::find()` for every container
- **Find time vs. n**, using `.find()`  for `set` and `unordered_set`.

Look at the *shape* of each line to understand how different containers behave.

# Files to Edit [Do not edit any other files]

containers.cpp\
Please edit the code only in the marked area (between `Your code starts from here` and `Your code ends here`) and do not edit anywhere else in this file. You must not edit any other file.

# Make Commands

```
make             # Build and Run Tests
make build       # Build
make runtests    # Run Tests
make plot        # Plot the growth curves (run this yourself, see above)
make clean       # Clean Temporary Files
```
Please read Makefile to understand the above commands!

# VS Code interface

You can open this folder in VS Code. The problem folder is configured
to enable debugging.

After opening the folder, click on Run > Start Debugging or press F5.
This will run your program on input ./test/test1/input.txt.
You may place breakpoints to pause the program at any desired location.

You can modify "./.vscode/launch.json" to run any test of your choice.

# Points to Ponder

- For `set` and `unordered_set`, how different are the `std::find()`
  and `.find()` lines? Does that gap grow, shrink, or stay the same
  as n grows?

- The `std::find()` lines for `vector`, `list`, `set`, and
  `unordered_set` are all doing the same kind of work (a linear
  scan). Do they all look similar on the graph, regardless of which
  container they're scanning?

- Does any single container win at *both* insertion and lookup, at
  every size -- assuming you always use the *right* search method for
  that container? Which one would you actually pick to log tens of
  thousands of file IDs while still answering lookups instantly?

# General Instructions

- Read all .h and .cpp files before starting.
- Do not include any additional header files.
- Do not modify files other than the specified files. Any other changes
  will not be considered during evaluation.
- You are expected to implement an efficient solution.
