#include "common.h"
#include "dequeblock.h"
#include "deque.h"

int main(int argc, char** argv) {
    if (argc > 1) {
        if (freopen(argv[1], "r", stdin) == nullptr) // cin redirects to file argv[1]
        {
            std::cerr << "Error: Could not open input file " << argv[1] << std::endl;
            return 1;
        }
    }


    ios_base::sync_with_stdio(false);
    cin.tie(nullptr);

    dequeBlock<int>* block = nullptr;
    Deque<int>* dq = nullptr;

    int q;
    cin >> q;

    for (int i = 0; i < q; i++) {
        string op;
        cin >> op;

        // ---------------- dequeBlock<int> commands ----------------
        if (op == "CREATE_BLOCK") {
            int cap;
            cin >> cap;
            delete block;
            block = new dequeBlock<int>(cap);
        }
        else if (op == "PUSH_BACK_BLOCK") {
            int val;
            cin >> val;
            cout << (block->pushBack(val) ? 1 : 0) << "\n";
        }
        else if (op == "PUSH_FRONT_BLOCK") {
            int val;
            cin >> val;
            cout << (block->pushFront(val) ? 1 : 0) << "\n";
        }
        else if (op == "POP_BACK_BLOCK") {
            int val;
            bool ok = block->popBack(val);
            cout << (ok ? val : -1) << "\n";
        }
        else if (op == "POP_FRONT_BLOCK") {
            int val;
            bool ok = block->popFront(val);
            cout << (ok ? val : -1) << "\n";
        }
        else if (op == "FRONT_BLOCK") {
            cout << (block->isEmpty() ? -1 : block->front()) << "\n";
        }
        else if (op == "BACK_BLOCK") {
            cout << (block->isEmpty() ? -1 : block->back()) << "\n";
        }
        else if (op == "AT_BLOCK") {
            int idx;
            cin >> idx;
            cout << (*block)[idx] << "\n";
        }
        else if (op == "SIZE_BLOCK") {
            cout << block->size() << "\n";
        }
        else if (op == "IS_FULL_BLOCK") {
            cout << (block->isFull() ? 1 : 0) << "\n";
        }
        else if (op == "IS_EMPTY_BLOCK") {
            cout << (block->isEmpty() ? 1 : 0) << "\n";
        }

        // ---------------- Deque<int> commands ----------------
        else if (op == "CREATE_DEQUE") {
            int blockCap;
            cin >> blockCap;
            delete dq;
            dq = new Deque<int>(blockCap);
        }
        else if (op == "PUSH_BACK") {
            int val;
            cin >> val;
            dq->pushBack(val);
        }
        else if (op == "PUSH_FRONT") {
            int val;
            cin >> val;
            dq->pushFront(val);
        }
        else if (op == "POP_BACK") {
            int val;
            bool ok = dq->popBack(val);
            cout << (ok ? val : -1) << "\n";
        }
        else if (op == "POP_FRONT") {
            int val;
            bool ok = dq->popFront(val);
            cout << (ok ? val : -1) << "\n";
        }
        else if (op == "GET_FRONT") {
            cout << (dq->isEmpty() ? -1 : dq->front()) << "\n";
        }
        else if (op == "GET_BACK") {
            cout << (dq->isEmpty() ? -1 : dq->back()) << "\n";
        }
        else if (op == "AT") {
            int idx;
            cin >> idx;
            cout << (*dq)[idx] << "\n";
        }
        else if (op == "SIZE") {
            cout << dq->size() << "\n";
        }
        else if (op == "IS_EMPTY") {
            cout << (dq->isEmpty() ? 1 : 0) << "\n";
        }
    }

    delete block;
    delete dq;
    return 0;
}
