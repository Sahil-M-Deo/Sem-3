#include <iostream>
#include <fstream>
#include "common.h"
#include "containers.h"

// ************************************************************
// DO NOT CHANGE ANYTHING IN THIS FILE
// ************************************************************
//
// Runs each of the four candidate implementations of the
// drift_runner log (see containers.cpp) at a range of log sizes, and
// prints how each one performed logging file IDs vs. checking them.

int main(int argc, char** argv) {
    if (argc > 1) {
        if (freopen(argv[1], "r", stdin) == nullptr) // cin redirects to file argv[1]
        {
            std::cerr << "Error: Could not open input file " << argv[1] << std::endl;
            return 1;
        }
    }

  unsigned n_max;   // largest log size to test
  unsigned step;    // step between successive log sizes
  unsigned repeat;  // number of repetitions per size, for averaging
  std::cin >> n_max;
  std::cin >> step;
  std::cin >> repeat;

  std::vector<unsigned> sizes;
  for (unsigned n = step; n <= n_max; n += step) {
    sizes.push_back(n);
  }

  ResultMap vec_r, list_r, set_r, uset_r;
  measure_vector(sizes, repeat, vec_r);
  measure_list(sizes, repeat, list_r);
  measure_set(sizes, repeat, set_r);
  measure_unordered_set(sizes, repeat, uset_r);

  // One row per (container, size):
  //   name size insert_us find_std_us find_member_us
  // find_member_us is -1 for containers with no member find() (vector, list).
  for (unsigned n : sizes) {
    std::cout << "vector "        << n << " " << vec_r[n].insert_us  << " " << vec_r[n].find_std_us  << " " << vec_r[n].find_member_us  << "\n";
  }
  for (unsigned n : sizes) {
    std::cout << "list "          << n << " " << list_r[n].insert_us << " " << list_r[n].find_std_us << " " << list_r[n].find_member_us << "\n";
  }
  for (unsigned n : sizes) {
    std::cout << "set "           << n << " " << set_r[n].insert_us  << " " << set_r[n].find_std_us  << " " << set_r[n].find_member_us  << "\n";
  }
  for (unsigned n : sizes) {
    std::cout << "unordered_set " << n << " " << uset_r[n].insert_us << " " << uset_r[n].find_std_us << " " << uset_r[n].find_member_us << "\n";
  }

  return 0;
}
