#ifndef SERVER_H
#define SERVER_H

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/strand.hpp>
#include <memory>
#include <mutex>
#include <thread>
#include "Messaging.h"
#include "ThreadPool.h"

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
using tcp = net::ip::tcp;

class Server {
public:
    Server(net::io_context& ioc, std::optional<tcp::endpoint> endpoint, std::shared_ptr<Messaging> messaging);
    virtual ~Server();
    void Start();
    void Stop();

protected:
    virtual void HandleRequest(tcp::socket socket);
    std::shared_ptr<Messaging> messaging_;

private:
    void Accept();

    net::io_context& ioc_;
    tcp::acceptor acceptor_;
    std::mutex mtx_;
    bool running_;
    std::thread serverThread_;
};

#endif // SERVER_H
