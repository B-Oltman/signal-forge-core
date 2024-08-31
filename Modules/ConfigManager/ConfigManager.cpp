#include "ConfigManager.h"
#include <chrono>
#include <stdexcept>
#include <algorithm>
#include <iostream>
#include "Logger.h"

ConfigManager::ConfigManager(std::string systemConfigPath)
    : systemConfigPath_(std::move(systemConfigPath)) {
    fileIO_ = std::make_shared<FileIO>();
}

nlohmann::json ConfigManager::ReadConfig(const std::string& filePath) {
    fileIO_->PushFileReadRequest(filePath);
    FileReadResponse response;
    while (!fileIO_->PopFileReadResponse(response)) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    return response.content;
}

void ConfigManager::WriteConfig(const std::string& filePath, const nlohmann::json& config) {
    fileIO_->PushFileWriteRequest(filePath, config);
    FileWriteResponse response;
    // Optionally handle response
}

bool ConfigManager::WriteConfigSync(const std::string& filePath, const nlohmann::json& content) {
    return fileIO_->WriteConfigSync(filePath, content);
}


std::string ConfigManager::GetSystemConfigPath() const {
    return systemConfigPath_;
}


nlohmann::json ConfigManager::ReadConfigSync(const std::string& filePath) {
    nlohmann::json content;
    if (!fileIO_->ReadConfigSync(filePath, content)) {
        Logger::Log("Failed to read configuration file: " + filePath, Logger::LogLevel::LOG_ERROR);        
        return nlohmann::json::object(); // Return empty JSON object if reading fails
    }
    return content;
}
