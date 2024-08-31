#include "MLTuningManagerServer.h"
#include "Logger.h"

void MLTuningManagerServer::HandleRequest(tcp::socket socket) {
    try {
        beast::tcp_stream stream(std::move(socket));
        beast::flat_buffer buffer;
        http::request<http::string_body> req;
        http::read(stream, buffer, req);

        if (req.method() == http::verb::post && req.target() == "/receive-data") {
            auto body = req.body();            
            messaging_->PushRequest(body);            

            http::response<http::string_body> res{http::status::ok, req.version()};
            res.set(http::field::server, "Boost.Beast");
            res.set(http::field::content_type, "application/json");
            res.body() = R"({"status":"Data received"})";
            res.prepare_payload();
            http::write(stream, res);
        }
    } catch (const std::exception& e) {
        Logger::Log("Error handling request: " + std::string(e.what()), Logger::LogLevel::LOG_ERROR);
    }
}
