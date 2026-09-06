#include "containers.h"

// ------------------------------------------------------------------
// TODO: std::vector
// ------------------------------------------------------------------
void measure_vector(const std::vector<unsigned>& sizes, unsigned repeat, ResultMap& results) {
  for (unsigned n : sizes) {
    Result r{0.0, 0.0, -1.0};

    for (unsigned t = 0; t < repeat; t++) {
      std::vector<int> v;

      // ---------------- measure insertion time ----------------
      auto start_time = std::chrono::steady_clock::now();

      // TODO: insert n file IDs into v

      // Your code starts from here -- DO NOT EDIT ANYTHING ABOVE
      for (unsigned i = 0; i < n; i++) {
        v.push_back(i);
      }
      // Your code ends here -- DO NOT EDIT ANYTHING BELOW

      auto duration = std::chrono::steady_clock::now() - start_time;
      long long time_taken =
          std::chrono::duration_cast<std::chrono::microseconds>(duration).count();
      r.insert_us += time_taken;

      // ---------------- measure std::find() time ----------------
      start_time = std::chrono::steady_clock::now();

      // TODO: check whether each of the n file IDs is in v, using std::find()
      
      // Your code starts from here -- DO NOT EDIT ANYTHING ABOVE
      for (unsigned i = 0; i < n; i++) {
        std::find(v.begin(), v.end(), (int)i);
      }
      // Your code ends here -- DO NOT EDIT ANYTHING BELOW

      duration = std::chrono::steady_clock::now() - start_time;
      time_taken =
          std::chrono::duration_cast<std::chrono::microseconds>(duration).count();
      r.find_std_us += time_taken;
    }

    r.insert_us /= repeat;
    r.find_std_us /= repeat;
    results[n] = r;
  }
}

// ------------------------------------------------------------------
// TODO: std::list
// ------------------------------------------------------------------
void measure_list(const std::vector<unsigned>& sizes, unsigned repeat, ResultMap& results) {
  
  // Your code starts from here -- DO NOT EDIT ANYTHING ABOVE
  for (unsigned n : sizes) {
    Result r{0.0, 0.0, -1.0};

    for (unsigned t = 0; t < repeat; t++) {
      std::list<int> l;

      // ---------------- measure insertion time ----------------
      auto start_time = std::chrono::steady_clock::now();

      for (unsigned i = 0; i < n; i++) {
        l.push_back(i);
      }

      auto duration = std::chrono::steady_clock::now() - start_time;
      long long time_taken =
          std::chrono::duration_cast<std::chrono::microseconds>(duration).count();
      r.insert_us += time_taken;

      // ---------------- measure std::find() time ----------------
      start_time = std::chrono::steady_clock::now();

      for (unsigned i = 0; i < n; i++) {
        std::find(l.begin(), l.end(), (int)i);
      }

      duration = std::chrono::steady_clock::now() - start_time;
      time_taken =
          std::chrono::duration_cast<std::chrono::microseconds>(duration).count();
      r.find_std_us += time_taken;
    }

    r.insert_us /= repeat;
    r.find_std_us /= repeat;
    results[n] = r;
  }
  // Your code ends here -- DO NOT EDIT ANYTHING BELOW

}

// ------------------------------------------------------------------
// TODO: std::set
//
// set has an internal tree structure (dw if you don't get what this means this will be covered later in class),
// so on top of the generic std::find() algorithm, it also offers its own member function find(),
// which can use that structure to search in O(log n) instead of doing a plain linear scan. 
// Measure BOTH, back to back, on the very same set of logged file IDs:
//   - find_std_us    : timing std::find(s.begin(), s.end(), i)
//   - find_member_us : timing s.find(i)
//
// They check for exactly the same file IDs in exactly the same
// container, so any difference between the two comes entirely from
// *how* the check is done.
// ------------------------------------------------------------------
void measure_set(const std::vector<unsigned>& sizes, unsigned repeat, ResultMap& results) {
  
  // Your code starts from here -- DO NOT EDIT ANYTHING ABOVE
  for (unsigned n : sizes) {
    Result r{0.0, 0.0, 0.0};

    for (unsigned t = 0; t < repeat; t++) {
      std::set<int> s;

      // ---------------- measure insertion time ----------------
      auto start_time = std::chrono::steady_clock::now();

      for (unsigned i = 0; i < n; i++) {
        s.insert(i);
      }

      auto duration = std::chrono::steady_clock::now() - start_time;
      long long time_taken =
          std::chrono::duration_cast<std::chrono::microseconds>(duration).count();
      r.insert_us += time_taken;

      // ---------------- measure std::find() time ----------------
      start_time = std::chrono::steady_clock::now();

      for (unsigned i = 0; i < n; i++) {
        std::find(s.begin(), s.end(), (int)i);
      }

      duration = std::chrono::steady_clock::now() - start_time;
      time_taken =
          std::chrono::duration_cast<std::chrono::microseconds>(duration).count();
      r.find_std_us += time_taken;

      // ---------------- measure member .find() time ----------------
      start_time = std::chrono::steady_clock::now();

      for (unsigned i = 0; i < n; i++) {
        s.find(i);
      }

      duration = std::chrono::steady_clock::now() - start_time;
      time_taken =
          std::chrono::duration_cast<std::chrono::microseconds>(duration).count();
      r.find_member_us += time_taken;
    }

    r.insert_us /= repeat;
    r.find_std_us /= repeat;
    r.find_member_us /= repeat;
    results[n] = r;
  }
  // Your code ends here -- DO NOT EDIT ANYTHING BELOW

}

// ------------------------------------------------------------------
// TODO: std::unordered_set
//
// Same idea as measure_set() above, but backed by a hash table (again, hash table will be covered later in class, dw)
// instead of a tree. Measure BOTH:
//   - std::find(us.begin(), us.end(), i)  -> a plain linear scan
//   - us.find(i)                          -> uses the hash table
// ------------------------------------------------------------------
void measure_unordered_set(const std::vector<unsigned>& sizes, unsigned repeat, ResultMap& results) {
  
  // Your code starts from here -- DO NOT EDIT ANYTHING ABOVE
  for (unsigned n : sizes) {
    Result r{0.0, 0.0, 0.0};

    for (unsigned t = 0; t < repeat; t++) {
      std::unordered_set<int> us;

      // ---------------- measure insertion time ----------------
      auto start_time = std::chrono::steady_clock::now();

      for (unsigned i = 0; i < n; i++) {
        us.insert(i);
      }

      auto duration = std::chrono::steady_clock::now() - start_time;
      long long time_taken =
          std::chrono::duration_cast<std::chrono::microseconds>(duration).count();
      r.insert_us += time_taken;

      // ---------------- measure std::find() time ----------------
      start_time = std::chrono::steady_clock::now();

      for (unsigned i = 0; i < n; i++) {
        std::find(us.begin(), us.end(), (int)i);
      }

      duration = std::chrono::steady_clock::now() - start_time;
      time_taken =
          std::chrono::duration_cast<std::chrono::microseconds>(duration).count();
      r.find_std_us += time_taken;

      // ---------------- measure member .find() time ----------------
      start_time = std::chrono::steady_clock::now();

      for (unsigned i = 0; i < n; i++) {
        us.find(i);
      }

      duration = std::chrono::steady_clock::now() - start_time;
      time_taken =
          std::chrono::duration_cast<std::chrono::microseconds>(duration).count();
      r.find_member_us += time_taken;
    }

    r.insert_us /= repeat;
    r.find_std_us /= repeat;
    r.find_member_us /= repeat;
    results[n] = r;
  }
  // Your code ends here -- DO NOT EDIT ANYTHING BELOW

}
