#include <iostream>
#include <memory>
#include <cstddef>

// NodePool definition
template <std::size_t N>
struct NodePool {
    double data[N]{};            // Data of node
    std::size_t next[N]{};       // User Defined Next Pointer
    bool is_free[N]{};           // Flag to describe node allocated or not
    std::size_t free_stack[N]{}; // Stack of unallocated nodes
    std::size_t top_free{};      // Top of free stack
};

// Allocate a node from the pool
template <std::size_t N>
void NodePool_allocate(NodePool<N> & pool, std::size_t & idx, int & status) noexcept {
    if (pool.top_free == 0) {
        status = -1; // No free nodes
        return;
    }
    idx = pool.free_stack[--pool.top_free];
    pool.is_free[idx] = false;
    status = 0;
}

// Deallocate a node and return it to the free stack
template <std::size_t N>
void NodePool_deallocate(NodePool<N> & pool, std::size_t & idx, int & status) noexcept {
    if (idx >= N || pool.is_free[idx]) {
        status = -1; // Invalid or already free
        return;
    }
    pool.is_free[idx] = true;
    pool.free_stack[pool.top_free++] = idx;
    status = 0;
}

// Set data in a node
template <std::size_t N>
void NodePool_setData(NodePool<N> & node, const std::size_t & idx, const double & val, int & status) noexcept {
    if (idx >= N || node.is_free[idx]) {
        status = -1;
        return;
    }
    node.data[idx] = val;
    status = 0;
}

// Set next pointer in a node
template <std::size_t N>
void NodePool_setNext(NodePool<N> & node, const std::size_t & idx, const size_t & val, int & status) noexcept {
    if (idx >= N || node.is_free[idx]) {
        status = -1;
        return;
    }
    node.next[idx] = val;
    status = 0;
}

// Get data from a node
template <std::size_t N>
void NodePool_getData(const NodePool<N> & node, const std::size_t & idx, double & val, int & status) noexcept {
    if (idx >= N || node.is_free[idx]) {
        status = -1;
        return;
    }
    val = node.data[idx];
    status = 0;
}

// Get next pointer from a node
template <std::size_t N>
void NodePool_getNext(NodePool<N> & node, const std::size_t & idx, size_t & val, int & status) noexcept {
    if (idx >= N || node.is_free[idx]) {
        status = -1;
        return;
    }
    val = node.next[idx];
    status = 0;
}

// Initialization routine
template <std::size_t N>
void NodePool_init(NodePool<N> & pool) noexcept {
    pool.top_free = N;
    for (std::size_t i = 0; i < N; ++i) {
        pool.is_free[i] = true;
        pool.free_stack[i] = N - i - 1; // Push in reverse order
    }
}

template <std::size_t N>
struct Stack {
    std::size_t top{};
    NodePool<N> pool{};
};

template <std::size_t N>
void Stack_init(Stack<N> & stack) noexcept {
    NodePool_init(stack.pool);
    stack.top = static_cast<std::size_t>(-1); // Empty stack
}

template <std::size_t N>
void Stack_push(Stack<N> & stack, const double & val, int & status) noexcept {
    std::size_t new_idx;
    NodePool_allocate(stack.pool, new_idx, status);
    if (status != 0)
        return;

    NodePool_setData(stack.pool, new_idx, val, status);
    if (status != 0)
        return;

    NodePool_setNext(stack.pool, new_idx, stack.top, status);
    if (status != 0)
        return;

    stack.top = new_idx;
}

template <std::size_t N>
void Stack_pop(Stack<N> & stack, double & val, int & status) noexcept {
    if (stack.top == static_cast<std::size_t>(-1)) {
        status = -1; // Stack underflow
        return;
    }

    std::size_t curr_top = stack.top;
    NodePool_getData(stack.pool, curr_top, val, status);
    if (status != 0)
        return;

    NodePool_getNext(stack.pool, curr_top, stack.top, status);
    if (status != 0)
        return;

    NodePool_deallocate(stack.pool, curr_top, status);
}

template <std::size_t N>
void Stack_peek(Stack<N> & stack, double & val, int & status) noexcept {
    if (stack.top == static_cast<std::size_t>(-1)) {
        status = -1; // Stack empty
        return;
    }

    NodePool_getData(stack.pool, stack.top, val, status);
}

int main() {
    Stack<10> stack;
    Stack_init(stack);

    int status;
    Stack_push(stack, 10.5, status);
    Stack_push(stack, 20.5, status);

    double val;
    Stack_peek(stack, val, status);
    if (status == 0)
        std::cout << "Peek: " << val << '\n';

    Stack_pop(stack, val, status);
    if (status == 0)
        std::cout << "Popped: " << val << '\n';

    Stack_pop(stack, val, status);
    if (status == 0)
        std::cout << "Popped: " << val << '\n';

    return 0;
}
