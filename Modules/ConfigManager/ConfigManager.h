#pragma once

#include <nlohmann/json.hpp>
#include <string>
#include <memory>
#include <vector>
#include <filesystem>
#include "FileIO.h"

class ConfigManager {
public:
    explicit ConfigManager(std::string systemConfigPath);

    nlohmann::json ReadConfig(const std::string& filePath);
    void WriteConfig(const std::string& filePath, const nlohmann::json& config);
    bool WriteConfigSync(const std::string& filePath, const nlohmann::json& content);        
    std::string GetSystemConfigPath() const;    
    nlohmann::json ReadConfigSync(const std::string& filePath);

private:
    std::string systemConfigPath_;
    std::shared_ptr<FileIO> fileIO_;
};
