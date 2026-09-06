#pragma once
#include <cstdio>
#include <iostream>
#include <string>
#include <vector>
using namespace std;

// One marker in the archive, stored as one node of a binary search tree.
struct Node {
    int key;
    Node *left;
    Node *right;
    Node(int k) : key(k), left(nullptr), right(nullptr) {}
};
