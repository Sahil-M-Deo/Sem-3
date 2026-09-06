#include "git-divergence.h"

Node* findDivergenceNaive(Node* headA, Node* headB) {
	// Your code starts from here -- DO NOT EDIT ANYTHING ABOVE
	// The first node of A that also lies on B is where they split.
	for (Node* a = headA; a != nullptr; a = a->next) {
		for (Node* b = headB; b != nullptr; b = b->next) {
			if (a == b) { // same node in memory
				return a;
			}
		}
	}

	return nullptr;
	// Your code ends here -- DO NOT EDIT ANYTHING BELOW

	return nullptr;
}

Node* findDivergenceOptimized(Node* headA, Node* headB) {
	// Your code starts from here -- DO NOT EDIT ANYTHING ABOVE
	int lenA = 0;
	for (Node* p = headA; p != nullptr; p = p->next) {
		lenA++;
	}

	int lenB = 0;
	for (Node* p = headB; p != nullptr; p = p->next) {
		lenB++;
	}

	// Drop the extra commits off the front of the longer branch.
	Node* a = headA;
	Node* b = headB;
	while (lenA > lenB) {
		a = a->next;
		lenA--;
	}
	while (lenB > lenA) {
		b = b->next;
		lenB--;
	}

	// Both are now the same distance from the end, so they meet at the commit
	// where the branches split, or at nullptr if there is no such commit.
	while (a != b) {
		a = a->next;
		b = b->next;
	}

	return a;
	// Your code ends here -- DO NOT EDIT ANYTHING BELOW

	return nullptr;
}
