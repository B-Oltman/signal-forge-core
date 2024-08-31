#include "DefaultLevelProcessor.h"

std::vector<TradeSignal> DefaultLevelProcessor::ProcessLevels(const std::unordered_map<double, std::vector<BaseLevel>>& levels, double currentPrice) {
    
    std::vector<TradeSignal> signals;
        
    // No-op implementation, you can add your processing logic here
    for (const auto& [price, levelList] : levels) {
        // Process each level
    }
    
    return signals;
}

std::vector<BaseLevel> DefaultLevelProcessor::GetLevelsToClear(const std::unordered_map<double, std::vector<BaseLevel>>& levels, double currentPrice)
{
    std::vector<BaseLevel> levelsToClear;
    
    // No-op implementation, you can add your processing logic here
    for (const auto& [price, levelList] : levels) {
        // Process each level
    }
    
    return levelsToClear;
}
