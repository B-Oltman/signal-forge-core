#include "MLTuningManager.h"
#include "ParameterManager.h"
#include "HttpClient.h"
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/strand.hpp>
#include <iostream>
#include <string>
#include <memory>
#include <mutex>
#include "Logger.h"

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
using tcp = net::ip::tcp;
using json = nlohmann::json;

MLTuningManager::MLTuningManager(std::shared_ptr<ITuningDataProcessor> tuner, ParameterManager& parameterManager, std::shared_ptr<Messaging> messaging)
    : tuner(tuner), parameterManager(parameterManager), running(true), ioc(1), httpClient(std::make_shared<HttpClient>()), server(std::make_shared<MLTuningManagerServer>(ioc, std::nullopt, messaging)), messaging_(messaging) {    

    messaging_->Subscribe([this](const std::string& data) {
        DataBlob dataBlob;
        dataBlob.key = "performance_data";
        dataBlob.data = json::parse(data);
        
        ProcessQueue(data);
    });
    
    server->Start();
}

MLTuningManager::~MLTuningManager() {
    running = false;
    cv.notify_all();
    server.reset();
    httpClient.reset();    
    messaging_.reset();
    tuner.reset();
}

void MLTuningManager::ProcessQueue(const std::string& request) {
    ThreadPool::Instance().Enqueue([this, request] {        
        DataBlob newData;                
        newData.key = "performance_data";
        newData.data = json::parse(request);                               
        
        auto mlInput = tuner->ProcessData(parameterManager.GetParameterGroup(), newData);

        // We can aggregate multiple blobs in ProcessData implementation. When we're ready to actually submit a request.
        // We would return the MachineLearningInput value to send along.
        if(mlInput.has_value())
        {
            SendDataToPythonServer(mlInput.value());
        }
    });
}

void MLTuningManager::SendDataToPythonServer(const MachineLearningInput& mlInput) {
    json j = mlInput.ToJson();
    std::string body = j.dump();

    httpClient->SendRequest("/process-data", body, [this](HttpResponse response) {
        if (response.success) {
            auto responseData = json::parse(response.content);
            // TODO: Some callback to alert system that tuned parameters are avilable.
            Logger::Log("ML Tuning Response Success", Logger::LogLevel::LOG_INFO);
        } else {
            Logger::Log("Error: " + response.content, Logger::LogLevel::LOG_ERROR);
        }
    });
}
