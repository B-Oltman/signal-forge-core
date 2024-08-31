#include "ThreadPool.h"

std::unique_ptr<ThreadPool> ThreadPool::instance;
std::once_flag ThreadPool::initFlag;

ThreadPool::ThreadPool(size_t numThreads) : pool(numThreads) {}

ThreadPool::~ThreadPool() {
    Join();
}

ThreadPool& ThreadPool::Instance(size_t numThreads) {
    std::call_once(initFlag, [&]() {
        instance.reset(new ThreadPool(numThreads));
    });
    return *instance;
}

void ThreadPool::Join() {
    pool.join();
}
