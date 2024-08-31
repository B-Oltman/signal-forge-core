#include "ParameterDataAccess.h"
#include "Logger.h"
#include "ParameterManager.h"
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/strand.hpp>
#include "ParameterDataAccessServer.h"

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
using tcp = net::ip::tcp;
using json = nlohmann::json;

ParameterDataAccess::ParameterDataAccess(std::shared_ptr<Messaging> messaging, ParameterManager& parameterManager)
    : parameterManager(parameterManager), messaging_(messaging), ioc_(), httpClient_(std::make_shared<HttpClient>()), server(std::make_shared<ParameterDataAccessServer>(ioc_, std::nullopt, messaging)) {

    server->Start();    
    messaging->Subscribe([this](const std::string& data) {
        json jsonData = json::parse(data);
        if (jsonData.contains("tradeSystemName")) {
            std::string tradeSystemName = jsonData["tradeSystemName"];
            std::optional<std::string> groupId = jsonData.contains("groupId") ? std::optional<std::string>(jsonData["groupId"].get<std::string>()) : std::nullopt;

            if (jsonData.contains("action") && jsonData["action"] == "update-parameter-group") {
                FetchParameterGroupFromDatabase(tradeSystemName, groupId);
            } else if (jsonData.contains("action") && jsonData["action"] == "update-trading-system") {
                FetchTradingSystemFromDatabase(tradeSystemName);
            }
        }
    });
}

ParameterDataAccess::~ParameterDataAccess() {
    running_ = false;    
    httpClient_.reset();
    messaging_.reset();
    ioc_.reset();
    server.reset();
}

void ParameterDataAccess::SendSessionToServer(const TradeSession &sessionResult) {
    json j = sessionResult.ToJson();
    std::string body = j.dump();

    httpClient_->SendRequest("/insert-session", body, [](HttpResponse response) {
        if (!response.success) {
            Logger::Log("Error: " + response.content, Logger::LogLevel::LOG_ERROR);
        }
    }, http::verb::post);
}

void ParameterDataAccess::FetchParameterGroupFromDatabase(const std::string &tradeSystemName, const std::optional<std::string> &groupId, bool block) {
    auto parameterGroupId = parameterManager.GetParameterGroup().id;
    auto currentTradeSystemName = parameterManager.GetSystemName();

    if (groupId.has_value() && parameterGroupId == groupId && currentTradeSystemName == tradeSystemName || currentTradeSystemName == tradeSystemName) {
        std::string url = "/get-parameter-groups?tradeSystemName=" + tradeSystemName + "&includeMetadata=true";
        if (groupId) {
            url += "&groupId=" + *groupId;
        } else {
            url += "&groupId=latest";
        }

        if (block) {
            std::promise<HttpResponse> promise;
            std::future<HttpResponse> future = promise.get_future();

            httpClient_->SendRequest(url, "", [&promise](HttpResponse response) {
                promise.set_value(response);
            }, http::verb::get);

            HttpResponse response = future.get();
            FetchParameterGroupCallback(response);
        } else {
            httpClient_->SendRequest(url, "", std::bind(&ParameterDataAccess::FetchParameterGroupCallback, this, std::placeholders::_1), http::verb::get);
        }
    }
}

void ParameterDataAccess::FetchParameterGroupCallback(const HttpResponse &response) {
    if (response.success) {
        try {
            json j = json::parse(response.content);
            if (j.empty()) {
                Logger::Log("Error: No parameter group returned from database.", Logger::LogLevel::LOG_ERROR);
            }
            auto parameterGroup = j.get<TradeSystemParameterGroup>();
            parameterManager.UpdateParameterGroup(parameterGroup);
        } catch (const json::exception &e) {
            Logger::Log("Error parsing parameter group: " + std::string(e.what()), Logger::LogLevel::LOG_ERROR);
        }
    } else {
        Logger::Log("Error fetching parameter group: " + response.content, Logger::LogLevel::LOG_ERROR);
    }
}

void ParameterDataAccess::FetchTradingSystemFromDatabase(const std::string &tradeSystemName, bool block) {
    auto currentTradeSystemName = parameterManager.GetSystemName();
    if (currentTradeSystemName == tradeSystemName) {
        std::string url = "/get-trading-systems?tradeSystemName=" + tradeSystemName;

        if (block) {
            std::promise<HttpResponse> promise;
            std::future<HttpResponse> future = promise.get_future();

            httpClient_->SendRequest(url, "", [&promise](HttpResponse response) {
                promise.set_value(response);
            }, http::verb::get);

            HttpResponse response = future.get();
            FetchTradingSystemCallback(response);
        } else {
            httpClient_->SendRequest(url, "", std::bind(&ParameterDataAccess::FetchTradingSystemCallback, this, std::placeholders::_1), http::verb::get);
        }
    }
}

void ParameterDataAccess::FetchTradingSystemCallback(const HttpResponse &response) {
    if (response.success) {
        try {
            json j = json::parse(response.content);
            if (j.empty() || !j.is_array() || j.size() == 0) {
                Logger::Log("Error: No trading system returned from database.", Logger::LogLevel::LOG_ERROR);
                return;
            }

            // Extract the first item in the array and parse it as a TradingSystem
            auto tradingSystem = j.at(0).get<TradingSystem>();  // Assuming TradingSystem is defined and can be parsed from JSON
            parameterManager.UpdateTradingSystem(tradingSystem);  // Assuming this function exists
        } catch (const json::exception &e) {
            Logger::Log("Error parsing trading system: " + std::string(e.what()), Logger::LogLevel::LOG_ERROR);
        }
    } else {
        Logger::Log("Error fetching trading system: " + response.content, Logger::LogLevel::LOG_ERROR);
    }
}

