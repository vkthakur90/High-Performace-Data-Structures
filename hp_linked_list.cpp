#include <iostream>
#include <cstddef>

// NodePool: POD managing a fixed pool of N nodes
template <std::size_t N>
struct NodePool {
    double data[N];                   // node values
    std::size_t next[N];              // next pointers (npos for end)
    bool is_free[N];                  // free flags
    std::size_t free_stack[N];        // stack of free node indices
    std::size_t top_free;             // stack pointer
};

// LinkedList: POD holding head, tail, and its NodePool
template <std::size_t N>
struct LinkedList {
    static constexpr std::size_t npos = N;
    std::size_t head;
    std::size_t tail;
    NodePool<N> pool;
};

// Initialize pool
// Function: NodePool_init
template <std::size_t N>
inline void NodePool_init(NodePool<N>& pool, int& status) noexcept {
    pool.top_free = N;
    for (std::size_t i = 0; i < N; ++i) {
        pool.is_free[i] = true;
        pool.free_stack[i] = N - i - 1;
    }
    status = 0;
}

// Allocate a node: NodePool_allocate
template <std::size_t N>
inline void NodePool_allocate(NodePool<N>& pool, std::size_t& idx, int& status) noexcept {
    if (pool.top_free == 0) {
        status = -1;
        return;
    }
    idx = pool.free_stack[--pool.top_free];
    pool.is_free[idx] = false;
    status = 0;
}

// Deallocate a node: NodePool_deallocate
template <std::size_t N>
inline void NodePool_deallocate(NodePool<N>& pool, std::size_t& idx, int& status) noexcept {
    if (idx >= N || pool.is_free[idx]) {
        status = -1;
        return;
    }
    pool.is_free[idx] = true;
    pool.free_stack[pool.top_free++] = idx;
    status = 0;
}

// Set data: NodePool_setData
template <std::size_t N>
inline void NodePool_setData(NodePool<N>& pool, const std::size_t& idx, const double& val, int& status) noexcept {
    if (idx >= N || pool.is_free[idx]) {
        status = -1;
        return;
    }
    pool.data[idx] = val;
    status = 0;
}

// Set next pointer: NodePool_setNext
template <std::size_t N>
inline void NodePool_setNext(NodePool<N>& pool, const std::size_t& idx, const std::size_t& nxt, int& status) noexcept {
    if (idx >= N || pool.is_free[idx]) {
        status = -1;
        return;
    }
    pool.next[idx] = nxt;
    status = 0;
}

// Get data: NodePool_getData
template <std::size_t N>
inline void NodePool_getData(const NodePool<N>& pool, const std::size_t& idx, double& val, int& status) noexcept {
    if (idx >= N || pool.is_free[idx]) {
        status = -1;
        return;
    }
    val = pool.data[idx];
    status = 0;
}

// Get next pointer: NodePool_getNext
template <std::size_t N>
inline void NodePool_getNext(const NodePool<N>& pool, const std::size_t& idx, std::size_t& nxt, int& status) noexcept {
    if (idx >= N || pool.is_free[idx]) {
        status = -1;
        return;
    }
    nxt = pool.next[idx];
    status = 0;
}

// Initialize linked list: LinkedList_init
template <std::size_t N>
inline void LinkedList_init(LinkedList<N>& list, int& status) noexcept {
    list.head = list.tail = LinkedList<N>::npos;
    NodePool_init(list.pool, status);
}

// pushFront: LinkedList_pushFront
template <std::size_t N>
inline void LinkedList_pushFront(LinkedList<N>& list, const double& value, int& status) noexcept {
    std::size_t idx;
    NodePool_allocate(list.pool, idx, status);
    if (status != 0) return;
    NodePool_setData(list.pool, idx, value, status);
    NodePool_setNext(list.pool, idx, list.head, status);
    list.head = idx;
    if (list.tail == LinkedList<N>::npos) {
        list.tail = idx;
    }
}

// pushBack: LinkedList_pushBack
template <std::size_t N>
inline void LinkedList_pushBack(LinkedList<N>& list, const double& value, int& status) noexcept {
    std::size_t idx;
    NodePool_allocate(list.pool, idx, status);
    if (status != 0) return;
    NodePool_setData(list.pool, idx, value, status);
    NodePool_setNext(list.pool, idx, LinkedList<N>::npos, status);
    if (list.head == LinkedList<N>::npos) {
        list.head = list.tail = idx;
    } else {
        NodePool_setNext(list.pool, list.tail, idx, status);
        list.tail = idx;
    }
}

// popFront: LinkedList_popFront
template <std::size_t N>
inline void LinkedList_popFront(LinkedList<N>& list, double& outVal, int& status) noexcept {
    if (list.head == LinkedList<N>::npos) {
        status = -1;
        return;
    }
    std::size_t oldHead = list.head;
    std::size_t nxt;
    NodePool_getData(list.pool, oldHead, outVal, status);
    if (status != 0) return;
    NodePool_getNext(list.pool, oldHead, nxt, status);
    list.head = nxt;
    if (list.head == LinkedList<N>::npos) {
        list.tail = LinkedList<N>::npos;
    }
    NodePool_deallocate(list.pool, oldHead, status);
}

// removeValue: LinkedList_removeValue
template <std::size_t N>
inline void LinkedList_removeValue(LinkedList<N>& list, const double& value, int& status) noexcept {
    if (list.head == LinkedList<N>::npos) {
        status = -1;
        return;
    }
    std::size_t curr = list.head;
    std::size_t prev = LinkedList<N>::npos;
    double currVal;
    while (curr != LinkedList<N>::npos) {
        NodePool_getData(list.pool, curr, currVal, status);
        if (status != 0) return;
        if (currVal == value) break;
        prev = curr;
        NodePool_getNext(list.pool, curr, curr, status);
    }
    if (curr == LinkedList<N>::npos) {
        status = -1;
        return;
    }
    std::size_t nxt;
    NodePool_getNext(list.pool, curr, nxt, status);
    if (prev == LinkedList<N>::npos) {
        list.head = nxt;
    } else {
        NodePool_setNext(list.pool, prev, nxt, status);
    }
    if (nxt == LinkedList<N>::npos) {
        list.tail = prev;
    }
    NodePool_deallocate(list.pool, curr, status);
}

// findValue: LinkedList_findValue
template <std::size_t N>
inline void LinkedList_findValue(const LinkedList<N>& list, const double& value, std::size_t& idx, int& status) noexcept {
    idx = list.head;
    double currVal;
    while (idx != LinkedList<N>::npos) {
        NodePool_getData(list.pool, idx, currVal, status);
        if (status != 0) return;
        if (currVal == value) {
            status = 0;
            return;
        }
        NodePool_getNext(list.pool, idx, idx, status);
    }
    status = -1;
}

// printList: LinkedList_printList
template <std::size_t N>
inline void LinkedList_printList(const LinkedList<N>& list) noexcept {
    std::size_t curr = list.head;
    double val;
    int status;
    std::cout << "LinkedList: ";
    while (curr != LinkedList<N>::npos) {
        NodePool_getData(list.pool, curr, val, status);
        std::cout << val;
        NodePool_getNext(list.pool, curr, curr, status);
        if (curr != LinkedList<N>::npos) std::cout << " -> ";
    }
    std::cout << "\n";
}

int main() {
    LinkedList<10> list;
    int status;

    LinkedList_init(list, status);

    LinkedList_pushBack(list, 1.1, status);
    LinkedList_pushFront(list, 0.5, status);
    LinkedList_pushBack(list, 2.2, status);
    LinkedList_printList(list); // 0.5 -> 1.1 -> 2.2

    double popped;
    LinkedList_popFront(list, popped, status);
    if (status == 0) std::cout << "Popped: " << popped << "\n";
    LinkedList_printList(list); // 1.1 -> 2.2

    LinkedList_removeValue(list, 1.1, status);
    LinkedList_printList(list); // 2.2

    std::size_t foundIdx;
    LinkedList_findValue(list, 2.2, foundIdx, status);
    if (status == 0) std::cout << "Found 2.2 at index " << foundIdx << "\n";

    return 0;
}
