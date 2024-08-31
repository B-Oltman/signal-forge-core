#pragma once

#include <nlohmann/json.hpp>
#include <string>
#include "FileIOMessaging.h"
#include "CommonTypes.h"

class FileIO {
public:
    FileIO();

    void PushFileReadRequest(const std::string& file_path);
    void PushFileWriteRequest(const std::string& file_path, const nlohmann::json& content);
    bool PopFileReadResponse(FileReadResponse& response);
    bool PopFileWriteResponse(FileWriteResponse& response);
    bool ReadConfigSync(const std::string& file_path, nlohmann::json& content);
    bool WriteConfigSync(const std::string& file_path, const nlohmann::json& content);

private:
    std::shared_ptr<FileIOMessaging> messaging_;
};
