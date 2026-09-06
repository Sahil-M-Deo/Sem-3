#include "common.h"
#include "linked-list.h"

// Your code starts from here -- DO NOT EDIT ANYTHING ABOVE

// Your code ends here -- DO NOT EDIT ANYTHING BELOW

int main(int argc, char** argv) {
	if (argc > 1) {
		if (freopen(argv[1], "r", stdin) == nullptr) // cin redirects to file argv[1]
		{
			std::cerr << "Error: Could not open input file " << argv[1] << std::endl;
			return 1;
		}
	}

	int n;
	cin >> n;

	LinkedList list;
	for (int i = 0; i < n; i++) {
		int x;
		cin >> x;
		list.insertSorted(x);
		list.print();
	}

	return 0;
}
