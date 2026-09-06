#include "list-inversion.h"

Node* reverse_list(Node* head) {
    // Your code starts from here -- DO NOT EDIT ANYTHING ABOVE
    Node *prev = nullptr;
    Node *curr = head;

    // Standard in-place reversal of the whole list: walk once from the head,
    // and as we visit each node flip its next pointer to point at the node we
    // just came from. prev trails one behind curr and, when curr falls off the
    // end, prev is sitting on the old last node -- the new head.
    while (curr != nullptr) {
        Node *nxt = curr->next; // remember the rest before we overwrite next
        curr->next = prev;      // relink this node to point backwards
        prev = curr;            // advance the trailing pointer
        curr = nxt;             // advance the leading pointer
    }

    return prev; // old tail, now the head of the reversed list
    // Your code ends here -- DO NOT EDIT ANYTHING BELOW
}
