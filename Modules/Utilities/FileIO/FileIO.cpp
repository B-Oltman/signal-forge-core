#include "FileIO.h"
#include <fstream>
#include <iostream>
#include "Logger.h"
#include "ThreadPool.h"

FileIO::FileIO()
    : messaging_(std::make_shared<FileIOMessaging>(Mode::Asynchronous)) {}

void FileIO::PushFileReadRequest(const std::string& file_path) {
    messaging_->PushFileReadRequest(file_path);
}

void FileIO::PushFileWriteRequest(const std::string& file_path, const nlohmann::json& content) {
    messaging_->PushFileWriteRequest(file_path, content);
}

bool FileIO::PopFileReadResponse(FileReadResponse& response) {
    return messaging_->PopFileReadResponse(response);
}

bool FileIO::PopFileWriteResponse(FileWriteResponse& response) {
    return messaging_->PopFileWriteResponse(response);
}

bool FileIO::ReadConfigSync(const std::string& file_path, nlohmann::json& content) {
    std::ifstream file(file_path);
    if (!file.is_open()) {
        Logger::Log("Failed to open file: " + file_path, Logger::LogLevel::LOG_ERROR);
        content = nlohmann::json::object(); // Return empty JSON object
        return false;
    }

    try {
        file >> content;
        file.close(); // Explicitly close the file
        return true;
    } catch (const nlohmann::json::parse_error& e) {
        Logger::Log("JSON parse error in file: " + file_path, Logger::LogLevel::LOG_ERROR);
        Logger::Log("Error message: " + std::string(e.what()), Logger::LogLevel::LOG_ERROR);
    } catch (const std::exception& e) {
        Logger::Log("Exception occurred while reading file: " + file_path, Logger::LogLevel::LOG_ERROR);
        Logger::Log("Error message: " + std::string(e.what()), Logger::LogLevel::LOG_ERROR);
    } catch (...) {
        Logger::Log("Unknown error occurred while reading file: " + file_path, Logger::LogLevel::LOG_ERROR);
    }

    file.close(); // Ensure the file is closed even if an exception is thrown
    content = nlohmann::json::object(); // Return empty JSON object
    return false;
}

bool FileIO::WriteConfigSync(const std::string& file_path, const nlohmann::json& content) {
    std::ofstream file(file_path);
    if (!file.is_open()) {
        Logger::Log("Failed to open file for writing: " + file_path, Logger::LogLevel::LOG_ERROR);
        return false;
    }

    try {
        file << content.dump(4); // Pretty-print JSON with indentation
        file.close();
        return true;
    } catch (const std::exception& e) {
        Logger::Log("Exception occurred while writing file: " + file_path, Logger::LogLevel::LOG_ERROR);
        Logger::Log("Error message: " + std::string(e.what()), Logger::LogLevel::LOG_ERROR);
    } catch (...) {
        Logger::Log("Unknown error occurred while writing file: " + file_path, Logger::LogLevel::LOG_ERROR);
    }

    file.close(); // Ensure the file is closed even if an exception is thrown
    return false;
}
