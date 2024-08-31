#include "Server.h"
#include "Logger.h"

Server::Server(net::io_context& ioc, std::optional<tcp::endpoint> endpoint, std::shared_ptr<Messaging> messaging)
    : ioc_(ioc), messaging_(messaging), running_(true), 
      acceptor_(ioc)  // Initialize the acceptor with io_context
{
    tcp::endpoint resolved_endpoint;

    if (endpoint.has_value()) {
        resolved_endpoint = endpoint.value();
    } else {
        resolved_endpoint = tcp::endpoint(net::ip::make_address("127.0.0.1"), 5005);
    }

    // Ensure the address and port have defaults if not explicitly provided
    if (resolved_endpoint.address().is_unspecified()) {
        resolved_endpoint = tcp::endpoint(net::ip::make_address("127.0.0.1"), resolved_endpoint.port());
    }
    if (resolved_endpoint.port() == 0) {
        resolved_endpoint = tcp::endpoint(resolved_endpoint.address(), 5005);
    }

    boost::system::error_code ec;
    acceptor_.open(resolved_endpoint.protocol(), ec);
    if (ec) {
        Logger::Log("Error opening acceptor: " + ec.message(), Logger::LogLevel::LOG_ERROR);
        return;
    }

    acceptor_.set_option(tcp::acceptor::reuse_address(true), ec);
    if (ec) {
        Logger::Log("Error setting reuse_address: " + ec.message(), Logger::LogLevel::LOG_ERROR);
        return;
    }

    acceptor_.bind(resolved_endpoint, ec);
    if (ec) {
        Logger::Log("Error binding acceptor: " + ec.message(), Logger::LogLevel::LOG_ERROR);
        return;
    }

    acceptor_.listen(net::socket_base::max_listen_connections, ec);
    if (ec) {
        Logger::Log("Error starting to listen: " + ec.message(), Logger::LogLevel::LOG_ERROR);
        return;
    }
}

Server::~Server() {
    Stop();
    messaging_.reset();
}

void Server::Start() {
    serverThread_ = std::thread([this] { Accept(); });
}

void Server::Stop() {
    {
        std::lock_guard<std::mutex> lock(mtx_);
        running_ = false;
    }
    boost::system::error_code ec;
    acceptor_.close(ec);  // Close the acceptor to stop the accept loop
    if (ec) {
        Logger::Log("Error closing acceptor: " + ec.message(), Logger::LogLevel::LOG_ERROR);
    }
    ioc_.stop();
    if (serverThread_.joinable()) {
        serverThread_.join();
    }
    Logger::Log("Server stopped.", Logger::LogLevel::LOG_INFO);
}

void Server::Accept() {
    while (running_) {
        std::shared_ptr<tcp::socket> socket = std::make_shared<tcp::socket>(ioc_);
        boost::system::error_code ec;
        acceptor_.accept(*socket, ec);
        if (ec) {
            if (!running_) {
                break;  // Exit the loop if the server is stopped
            }
            Logger::Log("Accept error: " + ec.message(), Logger::LogLevel::LOG_ERROR);
            continue;
        }
        ThreadPool::Instance().Enqueue([this, socket] {
            HandleRequest(std::move(*socket));
        });
    }
}

void Server::HandleRequest(tcp::socket socket) {
    // Base implementation (could be empty or generic)
}
