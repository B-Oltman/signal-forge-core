#pragma once

#include <string>
#include <functional>
#include <memory>
#include <thread>
#include <atomic>
#include "ThreadSafeQueue.h"
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <future>


class HttpResponse {
public:
    std::string content;
    bool success;
    HttpResponse(std::string c, bool s) : content(c), success(s) {}
};

class HttpClient {
public:
    HttpClient();
    ~HttpClient();

    void SendRequest(const std::string& url, const std::string& body, const std::function<void(HttpResponse)>& callback, 
                 boost::beast::http::verb method = boost::beast::http::verb::post, 
                 std::promise<HttpResponse>* promise = nullptr);

private:
    void ProcessRequests();
    std::atomic<bool> running_;
    std::thread processingThread_;

    ThreadSafeQueue<std::tuple<std::string, std::string, std::function<void(HttpResponse)>, boost::beast::http::verb>> requestQueue_;
};
