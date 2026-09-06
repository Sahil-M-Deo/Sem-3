#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <new>
#include <vector>

#include "circular.h"

namespace {

bool track_memory = false;
size_t allocations = 0;
size_t deallocations = 0;

void reset_memory_counts() {
    allocations = 0;
    deallocations = 0;
}

std::vector<node*> nodes(const list& value) {
    std::vector<node*> result;
    node* current = value.sentinel.next;
    while (current != &value.sentinel) {
        result.push_back(current);
        current = current->next;
    }
    return result;
}

void fill(list& value, const std::vector<int>& values) {
    for (int item : values) value.push_back(item);
}

bool matches(const list& value, const std::vector<int>& values,
             const std::vector<node*>* expected_nodes = nullptr) {
    const node* sentinel = &value.sentinel;
    const node* previous = sentinel;
    const node* current = sentinel->next;

    for (size_t i = 0; i < values.size(); ++i) {
        if (current == sentinel || current == nullptr) return false;
        if (expected_nodes != nullptr && current != (*expected_nodes)[i]) return false;
        if (current->data != values[i] || current->prev != previous) return false;
        previous = current;
        current = current->next;
    }

    if (current != sentinel || sentinel->prev != previous) return false;

    const node* next = sentinel;
    current = sentinel->prev;
    for (size_t i = values.size(); i > 0; --i) {
        if (current == sentinel || current == nullptr) return false;
        if (expected_nodes != nullptr && current != (*expected_nodes)[i - 1]) return false;
        if (current->data != values[i - 1] || current->next != next) return false;
        next = current;
        current = current->prev;
    }
    return current == sentinel && sentinel->next == next;
}

bool merge_case(const std::vector<int>& left_values,
                const std::vector<int>& right_values, bool check_constraints) {
    list left;
    list right;
    fill(left, left_values);
    fill(right, right_values);

    std::vector<int> expected_values = left_values;
    expected_values.insert(expected_values.end(), right_values.begin(), right_values.end());
    std::vector<node*> expected_nodes = nodes(left);
    std::vector<node*> right_nodes = nodes(right);
    expected_nodes.insert(expected_nodes.end(), right_nodes.begin(), right_nodes.end());

    reset_memory_counts();
    track_memory = true;
    left.merge(right);
    track_memory = false;

    if (!matches(left, expected_values)) return false;
    if (!matches(right, {})) return false;
    if (!check_constraints) return true;
    return allocations == 0 && deallocations == 0 &&
           matches(left, expected_values, &expected_nodes);
}

bool split_case(const std::vector<int>& values, int split_index,
                bool check_constraints) {
    list source;
    list result;
    fill(source, values);
    std::vector<node*> original_nodes = nodes(source);

    node* split_node = nullptr;
    size_t prefix_size = values.size();
    if (split_index == -2) {
        split_node = &source.sentinel;
    } else if (split_index >= 0) {
        prefix_size = static_cast<size_t>(split_index);
        split_node = original_nodes[prefix_size];
    }

    reset_memory_counts();
    track_memory = true;
    result = source.split(split_node);
    track_memory = false;

    std::vector<int> prefix(values.begin(), values.begin() + prefix_size);
    std::vector<int> suffix(values.begin() + prefix_size, values.end());
    if (!matches(source, prefix) || !matches(result, suffix)) return false;
    if (!check_constraints) return true;

    std::vector<node*> prefix_nodes(original_nodes.begin(), original_nodes.begin() + prefix_size);
    std::vector<node*> suffix_nodes(original_nodes.begin() + prefix_size, original_nodes.end());
    return allocations == 0 && deallocations == 0 &&
           matches(source, prefix, &prefix_nodes) &&
           matches(result, suffix, &suffix_nodes);
}

bool merge_complexity_case() {
    constexpr int list_size = 20000;
    constexpr int repeats = 50000;
    list left;
    list right;
    for (int i = 0; i < list_size; ++i) left.push_back(i);
    for (int i = 0; i < list_size; ++i) right.push_back(i + list_size);

    node* left_last = left.sentinel.prev;
    node* right_first = right.sentinel.next;
    node* right_last = right.sentinel.prev;

    reset_memory_counts();
    track_memory = true;
    for (int i = 0; i < repeats; ++i) {
        left.merge(right);
        if (left.sentinel.prev != right_last || left_last->next != right_first ||
            right_first->prev != left_last || right_last->next != &left.sentinel ||
            right.sentinel.next != &right.sentinel ||
            right.sentinel.prev != &right.sentinel) {
            track_memory = false;
            return false;
        }

        left.sentinel.prev = left_last;
        left_last->next = &left.sentinel;
        right.sentinel.next = right_first;
        right.sentinel.prev = right_last;
        right_first->prev = &right.sentinel;
        right_last->next = &right.sentinel;
    }
    track_memory = false;
    return allocations == 0 && deallocations == 0;
}

bool split_complexity_case() {
    constexpr int list_size = 40000;
    constexpr int repeats = 50000;
    list source;
    list tail;
    for (int i = 0; i < list_size; ++i) source.push_back(i);
    std::vector<node*> original_nodes = nodes(source);
    node* split_node = original_nodes[list_size / 2];
    node* prefix_last = split_node->prev;
    node* list_last = source.sentinel.prev;

    reset_memory_counts();
    track_memory = true;
    for (int i = 0; i < repeats; ++i) {
        tail = source.split(split_node);
        if (source.sentinel.prev != prefix_last || prefix_last->next != &source.sentinel ||
            tail.sentinel.next != split_node || tail.sentinel.prev != list_last ||
            split_node->prev != &tail.sentinel || list_last->next != &tail.sentinel) {
            track_memory = false;
            return false;
        }

        prefix_last->next = split_node;
        split_node->prev = prefix_last;
        source.sentinel.prev = list_last;
        list_last->next = &source.sentinel;
        tail.sentinel.next = &tail.sentinel;
        tail.sentinel.prev = &tail.sentinel;
    }
    track_memory = false;
    return allocations == 0 && deallocations == 0;
}

bool run_case(int test) {
    switch (test) {
        case 1: return merge_case({1, 2, 3}, {4, 5, 6}, false);
        case 2: return merge_case({1, 2, 3}, {}, false);
        case 3: return merge_case({}, {4, 5, 6}, false);
        case 4: return merge_case({1}, {2}, false);
        case 5: return merge_case({}, {}, false);
        case 6: return merge_case({-1, 0, -1}, {0, 7, 7}, false);
        case 7: return split_case({1, 2, 3, 4, 5}, 2, false);
        case 8: return split_case({1, 2, 3}, 0, false);
        case 9: return split_case({1, 2, 3}, 2, false);
        case 10: return split_case({1, 2, 3}, -1, false);
        case 11: return split_case({1, 2, 3}, -2, false);
        case 12: return split_case({9}, 0, false);
        case 13: return split_case({}, -1, false);
        case 14: return merge_case({1, 2, 3, 4}, {5, 6, 7}, true);
        case 15: return split_case({1, 2, 3, 4, 5, 6}, 3, true);
        case 16: return merge_complexity_case();
        case 17: return split_complexity_case();
        default: return false;
    }
}

}  // namespace

void* operator new(std::size_t size) {
    if (track_memory) ++allocations;
    if (void* memory = std::malloc(size)) return memory;
    throw std::bad_alloc();
}

void* operator new[](std::size_t size) {
    return ::operator new(size);
}

void operator delete(void* memory) noexcept {
    if (track_memory) ++deallocations;
    std::free(memory);
}

void operator delete[](void* memory) noexcept {
    ::operator delete(memory);
}

void operator delete(void* memory, std::size_t) noexcept {
    ::operator delete(memory);
}

void operator delete[](void* memory, std::size_t) noexcept {
    ::operator delete(memory);
}

int main(int argc, char* argv[]) {
    if (argc > 1 && std::freopen(argv[1], "r", stdin) == nullptr) return 1;

    int test = 0;
    if (!(std::cin >> test)) return 1;
    std::cout << (run_case(test) ? "OK\n" : "FAIL\n");
    std::cout.flush();
    std::_Exit(0);
}
