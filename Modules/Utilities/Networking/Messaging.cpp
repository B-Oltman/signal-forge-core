#include "Messaging.h"
#include "Logger.h"
#include <condition_variable>

Messaging::Messaging(Mode mode)
    : running_(true), mode_(mode) {
    if (mode_ == Mode::Asynchronous) {
        Start();
    }
}

Messaging::~Messaging() {
    Stop();
}

void Messaging::Subscribe(std::function<void(const std::string&)> callback) {
    std::lock_guard<std::mutex> lock(mtx_);
    subscribers_.push_back(callback);
}

void Messaging::PushRequest(const std::string& request) {
    {
        std::lock_guard<std::mutex> lock(mtx_);
        request_queue_.push(request);
    }
    cv_.notify_one();
    if (mode_ == Mode::Synchronous) {
        ProcessRequestsSynchronously();  // Process immediately if in synchronous mode
    }
}

bool Messaging::PopRequest(std::string& request) {
    if (!request_queue_.empty()) {
        request = request_queue_.front();
        request_queue_.pop();
        return true;
    }
    return false;
}

void Messaging::ProcessRequests() {
    while (running_) {
        std::unique_lock<std::mutex> lock(mtx_);
        cv_.wait(lock, [this] { return !running_ || !request_queue_.empty(); });

        if (!running_ && request_queue_.empty()) {
            break;
        }

        std::string request;
        if (PopRequest(request)) {
            for (const auto& callback : subscribers_) {
                callback(request);
            }
            Logger::Log("Processed request: " + request, Logger::LogLevel::LOG_INFO);
        }
    }
}

void Messaging::ProcessRequestsSynchronously() {
    std::string request;
    while (PopRequest(request)) {
        std::lock_guard<std::mutex> lock(mtx_); // Protect access to subscribers_
        for (const auto& callback : subscribers_) {
            callback(request);
        }
        Logger::Log("Processed request: " + request, Logger::LogLevel::LOG_INFO);
    }
}

void Messaging::Start() {
    processingThread_ = std::thread(&Messaging::ProcessRequests, this);
}

void Messaging::Stop() {
    {
        std::lock_guard<std::mutex> lock(mtx_);
        running_ = false;
    }
    cv_.notify_one();
    if (processingThread_.joinable()) {
        processingThread_.join();
    }
}
