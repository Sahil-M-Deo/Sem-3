#include <iostream>
#include <string>
#include <sstream>
#include "hash.h"

// Constructor
NestedList::NestedList(int size) : outerHead(nullptr), N(size) {
    if (size <= 0) return;

    outerHead = new OuterNode();
    OuterNode* current = outerHead;
    for (int i = 1; i < N; ++i) {
        current->next = new OuterNode();
        current = current->next;
    }
}

// Destructor
NestedList::~NestedList() {
    OuterNode* currentOuter = outerHead;
    while (currentOuter) {
        InnerNode* currentInner = currentOuter->head;
        while (currentInner) {
            InnerNode* tempInner = currentInner;
            currentInner = currentInner->next;
            delete tempInner;
        }
        OuterNode* tempOuter = currentOuter;
        currentOuter = currentOuter->next;
        delete tempOuter;
    }
}

// Display function
void NestedList::display() const {
    OuterNode* currentOuter = outerHead;
    while (currentOuter) {
        InnerNode* currentInner = currentOuter->head;
        while (currentInner) {
            std::cout << currentInner->value << " ";
            currentInner = currentInner->next;
        }
        std::cout << "\n";
        currentOuter = currentOuter->next;
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

    int N;
    std::string valuesInput;
    int searchVal;

    if (!(std::cin >> N)) return 0;

    std::string dummy;
    std::getline(std::cin, dummy);

    if (!std::getline(std::cin, valuesInput)) return 0;
    if (!(std::cin >> searchVal)) return 0;

    NestedList list(N);

    std::stringstream ss(valuesInput);
    int value;
    while (ss >> value) {
        list.insert(value);
    }

    list.display();
    std::cout << list.search(searchVal) << "\n";

    return 0;
}