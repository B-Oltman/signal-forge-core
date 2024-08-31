#ifndef FILEIO_MESSAGING_H
#define FILEIO_MESSAGING_H

#include "Messaging.h"
#include <nlohmann/json.hpp>

struct FileReadRequest {
    std::string file_path;
    FileReadRequest() = default;
    FileReadRequest(const std::string& path) : file_path(path) {}
};

struct FileReadResponse {
    std::string file_path;
    nlohmann::json content;
    bool success;
    FileReadResponse() = default;
    FileReadResponse(const std::string& path, bool success = false)
        : file_path(path), content(), success(success) {}
};

struct FileWriteRequest {
    std::string file_path;
    nlohmann::json content;
    FileWriteRequest() = default;
    FileWriteRequest(const std::string& path, const nlohmann::json& content)
        : file_path(path), content(content) {}
};

struct FileWriteResponse {
    std::string file_path;
    bool success;
    FileWriteResponse() = default;
    FileWriteResponse(const std::string& path, bool success = false)
        : file_path(path), success(success) {}
};

class FileIOMessaging : public Messaging {
public:
    FileIOMessaging(Mode mode);
    ~FileIOMessaging();

    void PushFileReadRequest(const std::string& file_path);
    void PushFileWriteRequest(const std::string& file_path, const nlohmann::json& content);
    bool PopFileReadResponse(FileReadResponse& response);
    bool PopFileWriteResponse(FileWriteResponse& response);

protected:
    void ProcessFileReadRequests();
    void ProcessFileWriteRequests();

private:
    boost::lockfree::spsc_queue<FileReadRequest, boost::lockfree::capacity<1024>> file_read_request_queue_;
    boost::lockfree::spsc_queue<FileReadResponse, boost::lockfree::capacity<1024>> file_read_response_queue_;
    boost::lockfree::spsc_queue<FileWriteRequest, boost::lockfree::capacity<1024>> file_write_request_queue_;
    boost::lockfree::spsc_queue<FileWriteResponse, boost::lockfree::capacity<1024>> file_write_response_queue_;
    std::thread readThread_;
    std::thread writeThread_;
};

#endif // FILEIO_MESSAGING_H
