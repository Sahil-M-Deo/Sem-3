#include <iostream>
#include <string>

#include "printmanager.h"

int main(int argc, char** argv) {
    if (argc > 1) {
        if (freopen(argv[1], "r", stdin) == nullptr)
        {
            std::cerr << "Error: Could not open input file " << argv[1] << std::endl;
            return 1;
        }
    }
    PrintManager P;
    std::string command;
    do {
        std::cin >> command;

        if (command == "NEW_DOC") {
            P.new_doc();
        } else if (command == "ADD_PAGE") {
            int x;
            std::cin >> x;
            P.add_page(x);
        } else if (command == "PRINT") {
            int k;
            std::cin >> k;
            P.print(k);
        } else if (command == "CANCEL") {
            P.cancel();
        } else if (command == "RECALL") {
            P.recall();
        } // else ignore unrecognized command
    } while (command != "EXIT");

    std::cout << std::endl;
}
