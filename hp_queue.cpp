#include <iostream>
#include <memory>
#include <cstddef>

// NodePool definition
template <std::size_t N>
struct NodePool {
    double data[N]{};
    std::size_t next[N]{};
    bool is_free[N]{};
    std::size_t free_stack[N]{};
    std::size_t top_free{};
};

template <std::size_t N>
void NodePool_init(NodePool<N> & pool) noexcept {
    pool.top_free = N;
    for (std::size_t i = 0; i < N; ++i) {
        pool.is_free[i] = true;
        pool.free_stack[i] = N - i - 1;
    }
}

template <std::size_t N>
void NodePool_allocate(NodePool<N> & pool, std::size_t & idx, int & status) noexcept {
    if (pool.top_free == 0) {
        status = -1;
        return;
    }
    idx = pool.free_stack[--pool.top_free];
    pool.is_free[idx] = false;
    status = 0;
}

template <std::size_t N>
void NodePool_deallocate(NodePool<N> & pool, std::size_t & idx, int & status) noexcept {
    if (idx >= N || pool.is_free[idx]) {
        status = -1;
        return;
    }
    pool.is_free[idx] = true;
    pool.free_stack[pool.top_free++] = idx;
    status = 0;
}

template <std::size_t N>
void NodePool_setData(NodePool<N> & node, const std::size_t & idx, const double & val, int & status) noexcept {
    if (idx >= N || node.is_free[idx]) {
        status = -1;
        return;
    }
    node.data[idx] = val;
    status = 0;
}

template <std::size_t N>
void NodePool_setNext(NodePool<N> & node, const std::size_t & idx, const size_t & val, int & status) noexcept {
    if (idx >= N || node.is_free[idx]) {
        status = -1;
        return;
    }
    node.next[idx] = val;
    status = 0;
}

template <std::size_t N>
void NodePool_getData(const NodePool<N> & node, const std::size_t & idx, double & val, int & status) noexcept {
    if (idx >= N || node.is_free[idx]) {
        status = -1;
        return;
    }
    val = node.data[idx];
    status = 0;
}

template <std::size_t N>
void NodePool_getNext(NodePool<N> & node, const std::size_t & idx, size_t & val, int & status) noexcept {
    if (idx >= N || node.is_free[idx]) {
        status = -1;
        return;
    }
    val = node.next[idx];
    status = 0;
}

template <std::size_t N>
struct Queue {
    std::size_t head{};
    std::size_t tail{};
    NodePool<N> pool{};
};

template <std::size_t N>
void Queue_init(Queue<N> & queue) noexcept {
    NodePool_init(queue.pool);
    queue.head = static_cast<std::size_t>(-1);
    queue.tail = static_cast<std::size_t>(-1);
}

template <std::size_t N>
void Queue_enqueue(Queue<N> & queue, const double & val, int & status) noexcept {
    std::size_t new_idx;
    NodePool_allocate(queue.pool, new_idx, status);
    if (status != 0) return;

    NodePool_setData(queue.pool, new_idx, val, status);
    if (status != 0) return;

    NodePool_setNext(queue.pool, new_idx, static_cast<std::size_t>(-1), status);
    if (status != 0) return;

    if (queue.tail != static_cast<std::size_t>(-1)) {
        NodePool_setNext(queue.pool, queue.tail, new_idx, status);
        if (status != 0) return;
    }
    queue.tail = new_idx;

    if (queue.head == static_cast<std::size_t>(-1)) {
        queue.head = new_idx;
    }
}

template <std::size_t N>
void Queue_dequeue(Queue<N> & queue, double & val, int & status) noexcept {
    if (queue.head == static_cast<std::size_t>(-1)) {
        status = -1;
        return;
    }

    std::size_t old_head = queue.head;
    NodePool_getData(queue.pool, old_head, val, status);
    if (status != 0) return;

    NodePool_getNext(queue.pool, old_head, queue.head, status);
    if (status != 0) return;

    if (queue.head == static_cast<std::size_t>(-1)) {
        queue.tail = static_cast<std::size_t>(-1);
    }

    NodePool_deallocate(queue.pool, old_head, status);
}

template <std::size_t N>
void Queue_peek(Queue<N> & queue, double & val, int & status) noexcept {
    if (queue.head == static_cast<std::size_t>(-1)) {
        status = -1;
        return;
    }
    NodePool_getData(queue.pool, queue.head, val, status);
}

int main() {
    Queue<10> queue;
    Queue_init(queue);

    int status;
    Queue_enqueue(queue, 1.1, status);
    Queue_enqueue(queue, 2.2, status);
    Queue_enqueue(queue, 3.3, status);

    double val;
    Queue_peek(queue, val, status);
    if (status == 0)
        std::cout << "Peek: " << val << '\n';

    while (true) {
        Queue_dequeue(queue, val, status);
        if (status != 0) break;
        std::cout << "Dequeued: " << val << '\n';
    }

    return 0;
}