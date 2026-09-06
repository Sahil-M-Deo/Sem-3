//
// Created by rishabh on 8/23/26.
//

#ifndef PRINTER_PRINTMANAGER_H
#define PRINTER_PRINTMANAGER_H
#include <deque>
#include <iostream>


// A struct for a single page to be printed
struct Page {
    int contents; // for simplicity content is just an int
    bool first_page; // indicates if this is the first page of a document
    bool is_first_page() const { return first_page; }
};

class PrintManager {
    std::deque<Page> D;

    // If true, the next page added is the first page of a new document.
    bool starting_new_doc = false;

    // sends one page to the printer.
    static void print_page(const Page& p) {
        std::cout << p.contents << " ";
    }

public:

    // A new document is about to start.
    void new_doc();

    // Add a new page with contents x to the current document.
    void add_page(int x);

    // The printer can currently print up to k pages.
    // Print as many waiting pages as possible, up to k.
    // Use the provided command print_page() to print each page
    void print(int k);

    // Cancel the document currently at the front, that is to be printed
    // or is being printed. Discard all of its unprinted pages.
    void cancel();

    // Cancel (recall) the most recently submitted document that
    // hasn't yet been recalled. Discard all of its unprinted pages.
    void recall();
};



#endif //PRINTER_PRINTMANAGER_H
