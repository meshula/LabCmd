#pragma once

#include <array>
#include <atomic>
#include <condition_variable>
#include <mutex>
#include <queue>



namespace polymer
{

    template<typename T>
    class mpsc_queue
    {
        struct buffer_node_t { T data; std::atomic<buffer_node_t*> next; };

        std::atomic<buffer_node_t*> _head;
        std::atomic<buffer_node_t*> _tail;

        mpsc_queue(const mpsc_queue &) { }
        void operator= (const mpsc_queue &) { }

    public:

        mpsc_queue() : _head(new buffer_node_t()), _tail(_head.load(std::memory_order_relaxed))
        {
            buffer_node_t * front = _head.load(std::memory_order_relaxed);
            front->next.store(nullptr, std::memory_order_relaxed);
        }

        ~mpsc_queue()
        {
            T output;
            while (!empty()) pop_front();
            buffer_node_t* front = _head.load(std::memory_order_relaxed);
            delete front;
        }

        bool emplace_back(T && input)
        {
            buffer_node_t* node = new buffer_node_t();
            node->data = std::move(input);
            node->next.store(nullptr, std::memory_order_relaxed);
            buffer_node_t* prevhead = _head.exchange(node, std::memory_order_acq_rel);
            prevhead->next.store(node, std::memory_order_release);
            return true;
        }

        bool emplace_back(const T & input)
        {
            buffer_node_t* node = new buffer_node_t();
            node->data = input;
            node->next.store(nullptr, std::memory_order_relaxed);
            buffer_node_t* prevhead = _head.exchange(node, std::memory_order_acq_rel);
            prevhead->next.store(node, std::memory_order_release);
            return true;
        }

        std::pair<bool, T> pop_front()
        {
            buffer_node_t * t = _tail.load(std::memory_order_relaxed);
            buffer_node_t * n = t->next.load(std::memory_order_acquire);
            if (n == nullptr) return { false, {} };
            std::pair<bool, T> output = { true, n->data };
            _tail.store(n, std::memory_order_release);
            delete t;
            return output;
        }

        bool empty()
        {
            buffer_node_t * tail = _tail.load(std::memory_order_relaxed);
            buffer_node_t * next = tail->next.load(std::memory_order_acquire);
            return next == nullptr;
        }
    };

    
    // inspired by https://github.com/dbittman/waitfree-mpsc-queue/blob/master/mpsc.c

    template <class T, size_t N>
    class mpsc_bounded_queue
    {
        std::array<T, N> _buffer;
        std::array<std::atomic<T*>, N> _ready_buffer;

        std::atomic<size_t> _count;
        size_t _tail;
        std::atomic<size_t> _head;

    public:
        mpsc_bounded_queue()
        : _count(0), _tail(0), _head(0) {}

        // thread safe
        bool emplace_back(T && val)
        {
            size_t count = _count.fetch_add(1, std::memory_order_acquire);
            if (count >= _buffer.size())
            {
                _count.fetch_sub(1, std::memory_order_release);
                return false;   // queue is full
            }

            // get exclusive access to head
            // relying on unsigned int wrap around to keep head increment atomic
            size_t h = _head.fetch_add(1, std::memory_order_acquire) % _buffer.size();
            _buffer[h] = std::move(val);

            // using a pointer to the element as a flag that the value is consumable
            _ready_buffer[h].store(&_buffer[h], std::memory_order_release);
            return true;
        }

        bool emplace_back(T const& val)
        {
            size_t count = _count.fetch_add(1, std::memory_order_acquire);
            if (count >= _buffer.size())
            {
                _count.fetch_sub(1, std::memory_order_release);
                return false;   // queue is full
            }

            // get exclusive access to head
            // relying on unsigned int wrap around to keep head increment atomic
            size_t h = _head.fetch_add(1, std::memory_order_acquire) % _buffer.size();
            _buffer[h] = val;

            // using a pointer to the element as a flag that the value is consumable
            _ready_buffer[h].store(&_buffer[h], std::memory_order_release);
            return true;
        }

        // safe on one thread only
        std::pair<bool, T> pop_front()
        {
            T* result = _ready_buffer[_tail].exchange(nullptr, std::memory_order_acquire);

            // result will be null if Enqueue is writing, or if the queue is empty
            if (!result)
                return { false, T{} };

            _tail = (_tail + 1) % _buffer.size();
            size_t count = _count.fetch_sub(1, std::memory_order_acquire);

            return { true, *result };
        }

        size_t size() const { return _count; }
        bool empty() const { return !_count; }
    };


    // This is free and unencumbered software released into the public domain.
    // Inspired by https://www.justsoftwaresolutions.co.uk/threading/implementing-a-thread-safe-queue-using-condition-variables.html

    template<typename T>
    class mpmc_queue_blocking
    {
        std::queue<T> queue;
        mutable std::mutex mutex;
        std::condition_variable condition;

        mpmc_queue_blocking(const mpmc_queue_blocking &) = delete;
        mpmc_queue_blocking & operator= (const mpmc_queue_blocking &) = delete;

    public:
        mpmc_queue_blocking() = default;

        // Emplace a new value and possibly notify one of the threads calling 'wait_and_consume'
        void emplace_back(T && value)
        {
            std::lock_guard<std::mutex> lock(mutex);
            queue.push(std::move(value));
            condition.notify_one();
        }

        void emplace_back(T const& value)
        {
            std::lock_guard<std::mutex> lock(mutex);
            queue.push(value);
            condition.notify_one();
        }

        // Blocking operation if queue is empty
        T wait_and_pop_front()
        {
            std::unique_lock<std::mutex> lock(mutex);
            while (queue.empty()) condition.wait(lock);
            auto popped_value = std::move(queue.front());
            queue.pop();
            return popped_value;
        }

        // Permits polling threads to do something else if the queue is empty
        std::pair<bool, T> pop_front()
        {
            std::lock_guard<std::mutex> lock(mutex);
            if (queue.empty()) return {false, {}};
            popped_value = std::move(queue.front());
            queue.pop();
            return {true, popped_value};
        }

        bool empty() const { std::unique_lock<std::mutex> lock(mutex); return queue.empty(); }
        std::size_t size() const { return queue.size(); }
    };


} // end namespace polymer


namespace Lab
{
    using polymer::mpsc_queue;
    using polymer::mpsc_bounded_queue;
    using polymer::mpmc_queue_blocking;
}
