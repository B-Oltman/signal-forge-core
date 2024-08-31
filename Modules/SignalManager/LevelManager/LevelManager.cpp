#include "LevelManager.h"
#include <algorithm>
#include <cmath>
#include <iostream>

LevelManager::LevelManager(std::vector<std::shared_ptr<LevelGenerator>> levelGenerators, std::shared_ptr<LevelProcessor> levelProcessor, std::shared_ptr<ITradingPlatform> tradingPlatform)
    : levelGenerators(std::move(levelGenerators)), levelProcessor(std::move(levelProcessor)), tp(tradingPlatform), running(true)
{
    mode_ = Mode::Synchronous;
    messaging_ = std::make_shared<Messaging>(mode_);
    if (mode_ == Mode::Asynchronous)
    {
        Start(); // Start the thread for async mode
    }
}

LevelManager::~LevelManager()
{
    Stop();
}

void LevelManager::SubscribeToMessages(std::function<void(const std::string &)> callback)
{
    messaging_->Subscribe(callback);
}

void LevelManager::Start()
{
    if (mode_ == Mode::Asynchronous)
    {
        processAndGenerateThread = std::thread(&LevelManager::Run, this); // Start the message processing thread
    }
}

void LevelManager::Stop()
{
    running = false;
    if (processAndGenerateThread.joinable())
    {
        processAndGenerateThread.join();
    }
}

void LevelManager::Run()
{
    while (running)
    {
        ProcessLevelsAndGenerateSignals();        
    }
}

void LevelManager::ProcessLevelsAndGenerateSignals()
{
    if (tp->IsReadyForTradeIteration())
    {
        double currentPrice = tp->GetCurrentPrice();
        ExecuteLevelProcessing(currentPrice);        
    }
}

void LevelManager::ExecuteLevelProcessing(double currentPrice)
{
    std::lock_guard<std::mutex> lock(levelsMutex);
    for (const auto &levelGenerator : levelGenerators)
    {
        auto generatedLevels = levelGenerator->GenerateLevels(currentPrice);
        for (const auto &level : generatedLevels)
        {
            levels[currentPrice].push_back(level);
        }
    }

    auto signals = levelProcessor->ProcessLevels(levels, currentPrice);
    for (const auto &signal : signals)
    {
        messaging_->PushRequest(signal.ToJsonString());
    }

    auto levelsToClear = levelProcessor->GetLevelsToClear(levels, currentPrice);
    ClearLevels(levelsToClear);
}

void LevelManager::ClearLevels(const std::vector<BaseLevel> &levelsToClear)
{
    for (const auto &level : levelsToClear)
    {
        auto it = levels.find(level.price);
        if (it != levels.end())
        {
            auto &levelList = it->second;
            levelList.erase(std::remove_if(levelList.begin(), levelList.end(), [&level](const BaseLevel &l)
                                           { return l == level; }),
                            levelList.end());
            if (levelList.empty())
            {
                levels.erase(it);
            }
        }
    }
}

bool LevelManager::IsWithinRange(double currentPrice, double levelPrice, double range)
{
    return std::abs(currentPrice - levelPrice) <= range;
}
