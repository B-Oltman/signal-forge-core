#ifndef PARAMETER_PERFORMANCE_MANAGER_H
#define PARAMETER_PERFORMANCE_MANAGER_H

#include <unordered_map>
#include <string>
#include <memory>
#include <atomic>
#include <condition_variable>
#include <chrono>
#include <queue>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/asio.hpp>
#include "CommonTypes.h"
#include "ITuningDataProcessor.h"
#include "ThreadSafeQueue.h"
#include "ThreadPool.h"
#include "HttpClient.h"
#include "MLTuningManagerServer.h"
#include "nlohmann/json.hpp"

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
using tcp = net::ip::tcp;
using json = nlohmann::json;

// Forward declaration to break circular dependency
class ParameterManager;

class MLTuningManager {
public:
    MLTuningManager(std::shared_ptr<ITuningDataProcessor> tuner, ParameterManager& parameterManager, std::shared_ptr<Messaging> messaging);
    ~MLTuningManager();

private:    
    void ProcessQueue(const std::string& request);
    void SendDataToPythonServer(const MachineLearningInput& mlInput);

    std::shared_ptr<ITuningDataProcessor> tuner;
    ParameterManager& parameterManager;
    std::atomic<bool> running;
    std::condition_variable cv;
    std::mutex mtx;
    net::io_context ioc;
    std::shared_ptr<HttpClient> httpClient;
    std::shared_ptr<Server> server;
    std::shared_ptr<Messaging> messaging_;        
};

#endif // PARAMETER_PERFORMANCE_MANAGER_H
