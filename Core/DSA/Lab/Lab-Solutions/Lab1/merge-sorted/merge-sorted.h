#pragma once

#include "common.h"

// A single node of a singly linked list.
struct node {
	int val;
	node* next = nullptr;
};

// A singly linked list, tracked via its head and tail pointers.
struct list {
	node* head = nullptr;
	node* tail = nullptr;

	bool empty() { return head == nullptr; }

	// Detaches and returns the front node of the list. Assumes the list is
	// non-empty.
	node* detach_front() {
		node* tmp = head;
		head = head->next;
		if (head == nullptr) tail = nullptr;
		tmp->next = nullptr;
		return tmp;
	}

	// Attaches p to the back of the list. Assumes p->next == nullptr.
	void attach_back(node* p) {
		if (tail) tail->next = p; else head = p;
		tail = p;
	}
};

// Merges L1 and L2, each already sorted in non-decreasing order by `val`,
// into a single new list that is also sorted in non-decreasing order, and
// returns a pointer to it.
//
// You must build the merged list by moving the existing nodes of L1 and L2
// (e.g. via detach_front/attach_back) -- do not allocate any new `node`s,
// and do not copy the values into an array/vector.
list* merge(list* L1, list* L2);
