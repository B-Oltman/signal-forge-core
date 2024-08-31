#pragma once

#include "LevelGenerator.h"
#include "LevelProcessor.h"
#include "Messaging.h"
#include "CommonTypes.h"
#include "ThreadPool.h"
#include <unordered_map>
#include <memory>
#include <vector>
#include <thread>
#include <mutex>

class LevelManager {
public:
    LevelManager(std::vector<std::shared_ptr<LevelGenerator>> levelGenerators, std::shared_ptr<LevelProcessor> levelProcessor, std::shared_ptr<ITradingPlatform> tradingPlatform);    
    void SubscribeToMessages(std::function<void(const std::string&)> callback);
    void ProcessLevelsAndGenerateSignals();
    std::vector<BaseLevel> GetLevelsInRange(double currentPrice, double range) const;
    void ClearExpiredLevels(const std::chrono::minutes& maxAge);
    void PrintCurrentLevels() const;

    virtual ~LevelManager();    

protected:
    void Start();
    void Stop();
    void Run();
    void ExecuteLevelProcessing(double currentPrice);
    void ClearLevels(const std::vector<BaseLevel>& levelsToClear);

    std::unordered_map<double, std::vector<BaseLevel>> levels;
    std::vector<std::shared_ptr<LevelGenerator>> levelGenerators;
    std::shared_ptr<LevelProcessor> levelProcessor;
    std::shared_ptr<ITradingPlatform> tp;
    std::shared_ptr<Messaging> messaging_;     
    std::thread processAndGenerateThread;
    std::mutex levelsMutex;  // Mutex to protect the levels map
    std::atomic<bool> running;
    Mode mode_;  // Mode to determine sync or async operation

    bool IsWithinRange(double currentPrice, double levelPrice, double range = 1.0);
};
