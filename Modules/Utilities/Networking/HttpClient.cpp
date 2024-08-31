#include "HttpClient.h"
#include "Logger.h"
#include "ThreadPool.h"
#include "CommonTypes.h"
#include "ParameterManager.h"
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/strand.hpp>
#include <future>


namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
using tcp = net::ip::tcp;

HttpClient::HttpClient()
    : running_(true),
      processingThread_(&HttpClient::ProcessRequests, this) {}

HttpClient::~HttpClient() {
    running_ = false;

    // Optionally push a dummy request to unblock the Pop in ProcessRequests
    requestQueue_.Push(std::make_tuple("", "", nullptr, http::verb::get));

    if (processingThread_.joinable()) {
        processingThread_.join();
    }
}

void HttpClient::SendRequest(const std::string& url, const std::string& body, const std::function<void(HttpResponse)>& callback, 
                             http::verb method, std::promise<HttpResponse>* promise) {
    requestQueue_.Push(std::make_tuple(url, body, [callback, promise](HttpResponse response) {
        if (callback) {
            callback(response);
        }
        if (promise) {
            promise->set_value(response);
        }
    }, method));
}

void HttpClient::ProcessRequests() {
    while (running_ || !requestQueue_.Empty()) {
        std::tuple<std::string, std::string, std::function<void(HttpResponse)>, http::verb> request;

        // Try to pop a request, but return immediately if running_ is false and the queue is empty
        if (!requestQueue_.Pop(request)) {
            if (!running_) {
                break;
            }
            continue;
        }

        auto [url, body, callback, method] = request;
        if (url.empty()) {
            continue; // Ignore dummy request
        }

        ThreadPool::Instance().Enqueue([url, body, callback, method] {
            try {
                net::io_context ioc;
                tcp::resolver resolver(ioc);                     

                auto defaultServerInfo = DataProcessingServer();
                defaultServerInfo.host = "localhost";
                defaultServerInfo.port = 5000;

                auto serverInfoValue = defaultServerInfo;            
                auto endpoints = resolver.resolve(serverInfoValue.host, std::to_string(serverInfoValue.port));
                beast::tcp_stream stream(ioc);
                stream.connect(endpoints);

                // Set up HTTP request
                http::request<http::string_body> req{method, url, 11};
                req.set(http::field::host, "localhost");
                if (method == http::verb::post) {
                    req.set(http::field::content_type, "application/json");
                    req.body() = body;
                    req.prepare_payload();
                }

                // Send HTTP request
                http::write(stream, req);

                // Read HTTP response
                beast::flat_buffer buffer;
                http::response<http::string_body> res;
                http::read(stream, buffer, res);

                if (res.result() == http::status::ok) {
                    callback(HttpResponse{res.body(), true});
                } else {
                    std::string errorMessage = "Error: Received response with status " + std::to_string(res.result_int()) + ", message: " + res.body();
                    callback(HttpResponse{errorMessage, false});
                }
            } catch (const std::exception& e) {
                Logger::Log("Error sending HTTP request: " + std::string(e.what()), Logger::LogLevel::LOG_ERROR);
                callback(HttpResponse{"Exception: " + std::string(e.what()), false});
            }
        });
    }
}
