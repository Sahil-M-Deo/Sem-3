#include "linked-list.h"



LinkedList::LinkedList() {
	// TODO: initialize an empty list.
    // Your code starts from here -- DO NOT EDIT ANYTHING ABOVE
	head = nullptr;
    // Your code ends here -- DO NOT EDIT ANYTHING BELOW

}

LinkedList::~LinkedList() {
	// TODO: free every node in the list to avoid memory leaks.
    // Your code starts from here -- DO NOT EDIT ANYTHING ABOVE
	Node* curr = head;
	while (curr != nullptr) {
		Node* next = curr->next;
		delete curr;
		curr = next;
	}
	head = nullptr;
    // Your code ends here -- DO NOT EDIT ANYTHING BELOW

}

void LinkedList::insertSorted(int value) {
	// TODO: insert value into the list, keeping it sorted in
	// non-decreasing order. Duplicate values should all be kept.
    // Your code starts from here -- DO NOT EDIT ANYTHING ABOVE
	Node* newNode = new Node(value);

	if (head == nullptr || value < head->data) {
		newNode->next = head;
		head = newNode;
		return;
	}

	Node* curr = head;
	while (curr->next != nullptr && curr->next->data <= value) {
		curr = curr->next;
	}
	newNode->next = curr->next;
	curr->next = newNode;
    // Your code ends here -- DO NOT EDIT ANYTHING BELOW

}

void LinkedList::print() const {
	// TODO: print the list's values in order, separated by a single
	// space, followed by a newline. Print just a newline if the list
	// is empty.
    // Your code starts from here -- DO NOT EDIT ANYTHING ABOVE
	Node* curr = head;
	bool first = true;
	while (curr != nullptr) {
		if (!first) {
			cout << " ";
		}
		cout << curr->data;
		first = false;
		curr = curr->next;
	}
	cout << endl;
    // Your code ends here -- DO NOT EDIT ANYTHING BELOW

}
