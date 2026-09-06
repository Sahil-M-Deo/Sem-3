// ************************************************************
// DO NOT CHANGE ANYTHING IN THIS FILE
// ************************************************************
#include "common.h"

struct Result {
  double insert_us;       // average time to insert `size` file IDs
  double find_std_us;     // average time to check `size` file IDs, via std::find()
  double find_member_us;  // average time to check `size` file IDs, via container.find()
                           // (-1 if this container has no such member function)
};

// Maps a log size -> the Result measured at that size.
using ResultMap = std::map<unsigned, Result>;
// the keyword "using" is used to create an alias for a type. 

// sizes  : the different log sizes to test (e.g. 500, 1000, 1500, ...)
// repeat : number of times each size is repeated, for averaging
// results: filled in by the function, one entry per size in `sizes`

void measure_vector(const std::vector<unsigned>& sizes, unsigned repeat, ResultMap& results);
void measure_list(const std::vector<unsigned>& sizes, unsigned repeat, ResultMap& results);
void measure_set(const std::vector<unsigned>& sizes, unsigned repeat, ResultMap& results);
void measure_unordered_set(const std::vector<unsigned>& sizes, unsigned repeat, ResultMap& results);
