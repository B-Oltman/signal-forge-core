
#include "LevelUtilities.h"
#include <iostream>
#include <algorithm>
#include <cmath>

namespace LevelUtilities {

    std::vector<BaseLevel> GetLevelsInRange(const std::unordered_map<double, std::vector<BaseLevel>>& levels, double currentPrice, double range) {
        std::vector<BaseLevel> levelsInRange;
        for (const auto& [price, levelList] : levels) {
            if (IsWithinRange(currentPrice, price, range)) {
                levelsInRange.insert(levelsInRange.end(), levelList.begin(), levelList.end());
            }
        }
        return levelsInRange;
    }
    
    void ClearExpiredLevels(std::unordered_map<double, std::vector<BaseLevel>>& levels, const std::chrono::minutes& maxAge) {
        auto now = std::chrono::system_clock::now();
        for (auto it = levels.begin(); it != levels.end();) {
            auto& levelList = it->second;
            levelList.erase(std::remove_if(levelList.begin(), levelList.end(), [now, maxAge](const BaseLevel& level) {
                return std::chrono::duration_cast<std::chrono::minutes>(now - level.timestamp.timePoint) > maxAge;
            }), levelList.end());
            if (levelList.empty()) {
                it = levels.erase(it);
            } else {
                ++it;
            }
        }
    }
    
    void PrintCurrentLevels(const std::unordered_map<double, std::vector<BaseLevel>>& levels) {
        for (const auto& [price, levelList] : levels) {
            std::cout << "Price: " << price << "\n";
            for (const auto& level : levelList) {
                std::cout << "  " << level.ToString() << "\n";
            }
        }
    }

    bool IsWithinRange(double currentPrice, double levelPrice, double range) {
        return std::abs(currentPrice - levelPrice) <= range;
    }
}
