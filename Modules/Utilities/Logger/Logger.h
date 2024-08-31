#pragma once
#include <string>
#include <atomic>
#include <boost/lockfree/queue.hpp>
#include <mutex>
#include <Windows.h>
#include <thread>
#include "CommonTypes.h" // Include the DateTime structure

class Logger {
public:
    enum class LogLevel {
        LOG_INFO,
        LOG_WARNING,
        LOG_ERROR
    };

    static Logger& Instance() {
        static Logger instance;
        return instance;
    }

    static void Log(const std::string& message, LogLevel level) {
        Instance().LogMessage(message, level);
    }

    ~Logger();

private:
    Logger();
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    struct LogEntry {
        std::string message;
        LogLevel level;
        DateTime timestamp; // Add timestamp to log entry
    };

    void LogMessage(const std::string& message, LogLevel level);
    void ProcessMessages();
    void Start();
    void Stop();
    void CreateDetachedConsole();
    void DisableConsoleCloseButton();

    boost::lockfree::queue<LogEntry*> messageQueue{1024};
    std::atomic<bool> running;
    std::atomic<bool> initialized;
    std::mutex logMutex;
    LogLevel currentLogLevel;

    HWND hwndConsole;
    static std::atomic<bool> consoleCreated;

    std::thread processingThread_;
};

