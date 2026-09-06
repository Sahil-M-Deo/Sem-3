#include "merge-sorted.h"

list* merge(list* L1, list* L2) {
	// TODO: merge L1 and L2 into a single new sorted list, and return a
	// pointer to it.
    // Your code starts from here -- DO NOT EDIT ANYTHING ABOVE
	list* mrg = new list;
	while (!L1->empty() || !L2->empty()) {
		bool use1 = !L1->empty() && (L2->empty() || (L1->head->val <= L2->head->val));
		list* src = use1 ? L1 : L2;
		mrg->attach_back(src->detach_front());
	}
	return mrg;
    // Your code ends here -- DO NOT EDIT ANYTHING BELOW

	return nullptr;
}
