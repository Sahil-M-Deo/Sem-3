#pragma once

#include "common.h"

// One commit. `next` points backwards in time, to the parent commit.
struct Node {
	int id;
	Node* next;

	Node(int value) : id(value), next(nullptr) {}
};

// Return the commit where feature-A and feature-B diverged, or nullptr if they
// share no history. Full specification in README.md.
Node* findDivergenceNaive(Node* headA, Node* headB);      // Part A, O(m * n)
Node* findDivergenceOptimized(Node* headA, Node* headB);  // Part B, O(m + n)
