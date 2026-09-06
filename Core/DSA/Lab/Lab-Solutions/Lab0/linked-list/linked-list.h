#pragma once

#include "common.h"

// A single node of the linked list.
struct Node {
	int data;
	Node* next;

	Node(int value) : data(value), next(nullptr) {}
};

class LinkedList {
private:
	Node* head;

public:
	LinkedList();
	~LinkedList();

	// Inserts value into the list, keeping the list sorted in
	// non-decreasing order. Duplicate values are allowed.
	void insertSorted(int value);

	// Prints the list's values, in order, separated by a single space,
	// followed by a newline. Prints just a newline if the list is empty.
	void print() const;
};
