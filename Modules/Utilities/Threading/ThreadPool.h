#pragma once
#include <boost/asio.hpp>
#include <boost/asio/thread_pool.hpp>
#include <boost/asio/post.hpp>
#include <memory>
#include <mutex>
#include <iostream>

class ThreadPool
{
public:
    static ThreadPool &Instance(size_t numThreads = 8); // Default to 8 threads
    ~ThreadPool();

    ThreadPool(const ThreadPool &) = delete;
    ThreadPool &operator=(const ThreadPool &) = delete;

    template <class F>
    void Enqueue(F &&f);

    void Join();

private:
    ThreadPool(size_t numThreads);

    static std::unique_ptr<ThreadPool> instance;
    static std::once_flag initFlag;

    boost::asio::thread_pool pool;
};

template <class F>
void ThreadPool::Enqueue(F &&f)
{
    boost::asio::post(pool, [func = std::forward<F>(f)]
                      { func(); });
}
