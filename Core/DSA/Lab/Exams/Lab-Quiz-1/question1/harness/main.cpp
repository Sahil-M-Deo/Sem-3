#include <iostream>
#include <string>
#include <sstream>
#include "circular.h"

int main(int argc, char* argv[]) {
    if (argc > 1) {
        if (freopen(argv[1], "r", stdin) == nullptr) {
            return 1;
        }
    }

    std::string line1, line2;
    int splitVal;

    if (!std::getline(std::cin, line1)) return 0;
    if (!std::getline(std::cin, line2)) return 0;
    if (!(std::cin >> splitVal)) return 0;

    list list1;
    list list2;

    std::stringstream ss1(line1);
    int val;
    while (ss1 >> val) {
        list1.push_back(val);
    }

    std::stringstream ss2(line2);
    while (ss2 >> val) {
        list2.push_back(val);
    }

    // 1. Perform Merge
    list1.merge(list2);
    list1.display();
    list2.display(); // Should print blank line (empty)

    // 2. Perform Split
    node* splitNode = list1.find(splitVal);
    list list3 = list1.split(splitNode);

    list1.display();
    list3.display();

    return 0;
}