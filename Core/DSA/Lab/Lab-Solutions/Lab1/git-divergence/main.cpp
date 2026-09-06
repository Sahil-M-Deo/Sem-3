#include "common.h"
#include "git-divergence.h"

static void print_commit(Node* commit) {
	if (commit == nullptr) {
		cout << "NONE";
	} else {
		cout << commit->id;
	}
}

int main(int argc, char** argv) {
	if (argc > 1) {
		if (freopen(argv[1], "r", stdin) == nullptr) // cin redirects to file argv[1]
		{
			std::cerr << "Error: Could not open input file " << argv[1] << std::endl;
			return 1;
		}
	}
	ios_base::sync_with_stdio(false);
	cin.tie(NULL);

	int q;
	cin >> q;
	while (q--) {
		int m, n, c;
		cin >> m >> n >> c;

		// Shared history, newest first. The first node read is where the
		// branches split, the last one is the project's first commit.
		Node* sharedHead = nullptr;
		Node* sharedTail = nullptr;
		for (int i = 0; i < c; i++) {
			int x;
			cin >> x;
			Node* node = new Node(x);
			if (sharedTail == nullptr) {
				sharedHead = node;
			} else {
				sharedTail->next = node;
			}
			sharedTail = node;
		}

		// Commits only on feature-A, HEAD first.
		Node* onlyAHead = nullptr;
		Node* onlyATail = nullptr;
		for (int i = 0; i < m; i++) {
			int x;
			cin >> x;
			Node* node = new Node(x);
			if (onlyATail == nullptr) {
				onlyAHead = node;
			} else {
				onlyATail->next = node;
			}
			onlyATail = node;
		}

		// Commits only on feature-B, HEAD first.
		Node* onlyBHead = nullptr;
		Node* onlyBTail = nullptr;
		for (int i = 0; i < n; i++) {
			int x;
			cin >> x;
			Node* node = new Node(x);
			if (onlyBTail == nullptr) {
				onlyBHead = node;
			} else {
				onlyBTail->next = node;
			}
			onlyBTail = node;
		}

		// Point both branches at the same shared nodes, which forms the Y.
		if (onlyATail != nullptr) {
			onlyATail->next = sharedHead;
		}
		if (onlyBTail != nullptr) {
			onlyBTail->next = sharedHead;
		}

		Node* headA = (onlyAHead != nullptr) ? onlyAHead : sharedHead;
		Node* headB = (onlyBHead != nullptr) ? onlyBHead : sharedHead;

		print_commit(findDivergenceNaive(headA, headB));
		cout << " ";
		print_commit(findDivergenceOptimized(headA, headB));
		cout << "\n";

		// Free each node once: the two exclusive runs, then the shared tail.
		Node* cur = onlyAHead;
		for (int i = 0; i < m; i++) {
			Node* nxt = cur->next;
			delete cur;
			cur = nxt;
		}
		cur = onlyBHead;
		for (int i = 0; i < n; i++) {
			Node* nxt = cur->next;
			delete cur;
			cur = nxt;
		}
		cur = sharedHead;
		for (int i = 0; i < c; i++) {
			Node* nxt = cur->next;
			delete cur;
			cur = nxt;
		}
	}

	return 0;
}
