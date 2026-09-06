#include "common.h"
#include "merge-sorted.h"

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

	int n1;
	cin >> n1;
	list* L1 = new list;
	for (int i = 0; i < n1; i++) {
		int x;
		cin >> x;
		L1->attach_back(new node{x});
	}

	int n2;
	cin >> n2;
	list* L2 = new list;
	for (int i = 0; i < n2; i++) {
		int x;
		cin >> x;
		L2->attach_back(new node{x});
	}

	list* mrg = merge(L1, L2);

	bool first = true;
	for (node* curr = mrg->head; curr != nullptr; curr = curr->next) {
		if (!first) {
			cout << " ";
		}
		cout << curr->val;
		first = false;
	}
	cout << endl;

	while (!mrg->empty()) {
		delete mrg->detach_front();
	}
	delete mrg;
	delete L1;
	delete L2;

	return 0;
}
