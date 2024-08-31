#pragma once

#include "CommonTypes.h"
#include <vector>
#include <unordered_map>

namespace LevelUtilities {
    
    std::vector<BaseLevel> GetLevelsInRange(const std::unordered_map<double, std::vector<BaseLevel>>& levels, double currentPrice, double range);
    
    void ClearExpiredLevels(std::unordered_map<double, std::vector<BaseLevel>>& levels, const std::chrono::minutes& maxAge);
    
    void PrintCurrentLevels(const std::unordered_map<double, std::vector<BaseLevel>>& levels);

    bool IsWithinRange(double currentPrice, double levelPrice, double range);
}
