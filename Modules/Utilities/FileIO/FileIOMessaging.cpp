#include "FileIOMessaging.h"
#include "Logger.h"
#include <fstream>

FileIOMessaging::FileIOMessaging(Mode mode)
    : Messaging(mode) {
    if (mode == Mode::Asynchronous) {
        readThread_ = std::thread(&FileIOMessaging::ProcessFileReadRequests, this);
        writeThread_ = std::thread(&FileIOMessaging::ProcessFileWriteRequests, this);
    }
}

FileIOMessaging::~FileIOMessaging() {
    running_.store(false);
    if (readThread_.joinable()) {
        readThread_.join();
    }
    if (writeThread_.joinable()) {
        writeThread_.join();
    }
}

void FileIOMessaging::PushFileReadRequest(const std::string& file_path) {
    FileReadRequest request(file_path);
    file_read_request_queue_.push(request);
}

void FileIOMessaging::PushFileWriteRequest(const std::string& file_path, const nlohmann::json& content) {
    FileWriteRequest request(file_path, content);
    file_write_request_queue_.push(request);
}

bool FileIOMessaging::PopFileReadResponse(FileReadResponse& response) {
    return file_read_response_queue_.pop(response);
}

bool FileIOMessaging::PopFileWriteResponse(FileWriteResponse& response) {
    return file_write_response_queue_.pop(response);
}

void FileIOMessaging::ProcessFileReadRequests() {
    while (running_.load()) {
        FileReadRequest request;
        if (file_read_request_queue_.pop(request)) {
            FileReadResponse response(request.file_path, false);
            try {
                std::ifstream file(request.file_path);
                if (file.is_open()) {
                    file >> response.content;
                    response.success = true;
                } else {
                    Logger::Log("Failed to open file: " + request.file_path, Logger::LogLevel::LOG_ERROR);
                }
            } catch (const std::exception& e) {
                Logger::Log("Exception occurred while reading file: " + request.file_path, Logger::LogLevel::LOG_ERROR);
                Logger::Log("Error message: " + std::string(e.what()), Logger::LogLevel::LOG_ERROR);
            } catch (...) {
                Logger::Log("Unknown error occurred while reading file: " + request.file_path, Logger::LogLevel::LOG_ERROR);
            }
            file_read_response_queue_.push(response);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

void FileIOMessaging::ProcessFileWriteRequests() {
    while (running_.load()) {
        FileWriteRequest request;
        if (file_write_request_queue_.pop(request)) {
            FileWriteResponse response(request.file_path, false);
            try {
                std::ofstream file(request.file_path);
                if (file.is_open()) {
                    file << request.content.dump(4); // Pretty-print JSON with indentation
                    response.success = true;
                } else {
                    Logger::Log("Failed to open file for writing: " + request.file_path, Logger::LogLevel::LOG_ERROR);
                }
            } catch (const std::exception& e) {
                Logger::Log("Exception occurred while writing file: " + request.file_path, Logger::LogLevel::LOG_ERROR);
                Logger::Log("Error message: " + std::string(e.what()), Logger::LogLevel::LOG_ERROR);
            } catch (...) {
                Logger::Log("Unknown error occurred while writing file: " + request.file_path, Logger::LogLevel::LOG_ERROR);
            }
            file_write_response_queue_.push(response);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}
