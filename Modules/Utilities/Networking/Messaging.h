#ifndef MESSAGING_H
#define MESSAGING_H

#include <queue>
#include <mutex>
#include <string>
#include <atomic>
#include <thread>
#include <functional>
#include <vector>
#include <condition_variable>  // Add this include
#include <boost/lockfree/spsc_queue.hpp>
#include "ThreadPool.h"
#include "CommonTypes.h"

class Messaging {
public:
    Messaging(Mode mode = Mode::Asynchronous);  // Add mode parameter to constructor
    ~Messaging();

    void PushRequest(const std::string& request);
    bool PopRequest(std::string& request);

    void Subscribe(std::function<void(const std::string&)> callback);

protected:
    void ProcessRequests();
    void ProcessRequestsSynchronously();  // Add synchronous processing method

    std::atomic<bool> running_;
    std::vector<std::function<void(const std::string&)>> subscribers_;
    boost::lockfree::spsc_queue<std::string, boost::lockfree::capacity<1024>> request_queue_;
    std::mutex mtx_;
    std::condition_variable cv_;  // Add condition variable
    std::thread processingThread_;
    Mode mode_;  // Add mode member

private:
    void Start();
    void Stop();
};

#endif // MESSAGING_H
