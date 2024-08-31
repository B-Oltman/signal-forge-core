#include "ParameterDataAccessServer.h"
#include "Logger.h"
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <nlohmann/json.hpp>

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
using tcp = net::ip::tcp;
using json = nlohmann::json;

void ParameterDataAccessServer::HandleRequest(tcp::socket socket) {
    try {
        beast::tcp_stream stream(std::move(socket));
        beast::flat_buffer buffer;
        http::request<http::string_body> req;
        http::read(stream, buffer, req);

        if (req.method() == http::verb::post && req.target() == "/update-parameter-group") {
            auto body = req.body();
            json wrappedMessage = {
                {"action", "update-parameter-group"},
                {"tradeSystemName", ""},
                {"groupId", ""}
            };

            // Parse the body to extract relevant fields
            json bodyJson = json::parse(body);
            if (bodyJson.contains("tradeSystemName")) {
                wrappedMessage["tradeSystemName"] = bodyJson["tradeSystemName"];
            }
            if (bodyJson.contains("groupId")) {
                wrappedMessage["groupId"] = bodyJson["groupId"];
            }

            messaging_->PushRequest(wrappedMessage.dump());

            http::response<http::string_body> res{http::status::ok, req.version()};
            res.set(http::field::server, "Boost.Beast");
            res.set(http::field::content_type, "application/json");
            res.body() = R"({"status":"Parameter group update triggered"})";
            res.prepare_payload();
            http::write(stream, res);
        } else if (req.method() == http::verb::post && req.target() == "/update-trading-system") {
            auto body = req.body();
            json wrappedMessage = {
                {"action", "update-trading-system"},
                {"tradeSystemName", ""}
            };

            // Parse the body to extract relevant fields
            json bodyJson = json::parse(body);
            if (bodyJson.contains("tradeSystemName")) {
                wrappedMessage["tradeSystemName"] = bodyJson["tradeSystemName"];
            }

            messaging_->PushRequest(wrappedMessage.dump());

            http::response<http::string_body> res{http::status::ok, req.version()};
            res.set(http::field::server, "Boost.Beast");
            res.set(http::field::content_type, "application/json");
            res.body() = R"({"status":"Trading system update triggered"})";
            res.prepare_payload();
            http::write(stream, res);
        }
    } catch (const std::exception& e) {
        Logger::Log("Error handling request: " + std::string(e.what()), Logger::LogLevel::LOG_ERROR);
    }
}
