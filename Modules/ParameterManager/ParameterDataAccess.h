#ifndef PARAMETER_DATA_ACCESS_H
#define PARAMETER_DATA_ACCESS_H

#include <memory>
#include <string>
#include <mutex>
#include <thread>
#include <atomic>
#include <functional>
#include <boost/asio.hpp>
#include "CommonTypes.h"
#include "HttpClient.h"
#include "Messaging.h"
#include "Server.h"

class ParameterManager;

class ParameterDataAccess {
public:
    void SendSessionToServer(const TradeSession& sessionResult);
    void FetchParameterGroupFromDatabase(const std::string& tradeSystemName, const std::optional<std::string>& groupId = std::nullopt, bool block = false);
    void FetchTradingSystemFromDatabase(const std::string& tradeSystemName, bool block = false);

    ParameterDataAccess(std::shared_ptr<Messaging> messaging, ParameterManager& parameterManager);
    ~ParameterDataAccess();
private:

    void FetchParameterGroupCallback(const HttpResponse& response);
    void FetchTradingSystemCallback(const HttpResponse& response);

    ParameterManager& parameterManager;
    std::shared_ptr<Messaging> messaging_;
    std::shared_ptr<HttpClient> httpClient_;
    std::atomic<bool> running_;
    net::io_context ioc_;
    std::shared_ptr<Server> server;    

    static std::shared_ptr<ParameterDataAccess> instance_;
    static std::mutex instanceMutex_;
};

#endif // PARAMETER_DATA_ACCESS_H
