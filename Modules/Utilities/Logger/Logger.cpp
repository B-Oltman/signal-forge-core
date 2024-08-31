#include "Logger.h"
#include <iostream>
#include <fstream>
#include "CommonTypes.h"

std::atomic<bool> Logger::consoleCreated = false;

Logger::Logger() : running(false), initialized(false), currentLogLevel(LogLevel::LOG_INFO) {
    Start();
}

Logger::~Logger() {
    Stop();
}

void Logger::Start() {
    if (!initialized.exchange(true)) {
        running.store(true);

        CreateDetachedConsole();
        DisableConsoleCloseButton();
        SetConsoleTitleA("Trade System Logs");

        // Redirect cout and cerr to the console
        FILE* fp;
        freopen_s(&fp, "CONOUT$", "w", stdout);
        freopen_s(&fp, "CONOUT$", "w", stderr);

        // Start the dedicated thread for processing log messages
        processingThread_ = std::thread(&Logger::ProcessMessages, this);
    }
}

void Logger::Stop() {
    running.store(false);
    if (processingThread_.joinable()) {
        processingThread_.join();
    }

    // Close the console if it was created by the logger
    if (consoleCreated) {
        FreeConsole();
    }
}

void Logger::LogMessage(const std::string& message, LogLevel level) {
    if (!initialized.load()) {
        Start();
    }

    LogEntry* entry = new LogEntry{message, level, DateTime()}; // Include timestamp
    while (!messageQueue.push(entry)) {
        // If the queue is full, you might want to add a backoff strategy here
    }
}

void Logger::ProcessMessages() {
    LogEntry* entry;
    while (running.load() || !messageQueue.empty()) {        
        if (messageQueue.pop(entry)) {
            std::lock_guard<std::mutex> lock(logMutex);
            if (entry->level < currentLogLevel) {
                delete entry;
                continue;
            }

            std::string timestamp = entry->timestamp.ToString(); // Get timestamp string
            std::string tradeSystem = GetTradeSystemName();

            switch (entry->level) {
                case LogLevel::LOG_INFO:
                    std::cout << tradeSystem + ": " << timestamp << " INFO: " << entry->message << std::endl;
                    break;
                case LogLevel::LOG_WARNING:
                    std::cout << tradeSystem + ": " << timestamp << " WARNING: " << entry->message << std::endl;
                    break;
                case LogLevel::LOG_ERROR:
                    std::cerr << tradeSystem + ": " << timestamp << " ERROR: " << entry->message << std::endl;
                    break;
            }
            delete entry;
        } else {
            // Prevent busy waiting
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }

    // Ensure all remaining messages are processed before exiting
    while (messageQueue.pop(entry)) {
        std::lock_guard<std::mutex> lock(logMutex);
        if (entry->level < currentLogLevel) {
            delete entry;
            continue;
        }

        std::string timestamp = entry->timestamp.ToString(); // Get timestamp string

        switch (entry->level) {
            case LogLevel::LOG_INFO:
                std::cout << timestamp << " INFO: " << entry->message << std::endl;
                break;
            case LogLevel::LOG_WARNING:
                std::cout << timestamp << " WARNING: " << entry->message << std::endl;
                break;
            case LogLevel::LOG_ERROR:
                std::cerr << timestamp << " ERROR: " << entry->message << std::endl;
                break;
        }
        delete entry;
    }
}

void Logger::CreateDetachedConsole() {
    // Create a detached console window
    if (!AllocConsole()) {
        if (GetLastError() == ERROR_ACCESS_DENIED) {
            AttachConsole(ATTACH_PARENT_PROCESS);
        }
    } else {
        consoleCreated = true;
    }

    // Set console close handler
    SetConsoleCtrlHandler([](DWORD event) -> BOOL {
        if (event == CTRL_CLOSE_EVENT) {
            // Ignore the close event to keep the application running
            FreeConsole();
            return TRUE;
        }
        return FALSE;
    }, TRUE);
}

void Logger::DisableConsoleCloseButton() {
    HWND hwnd = GetConsoleWindow();
    if (hwnd != NULL) {
        HMENU hMenu = GetSystemMenu(hwnd, FALSE);
        if (hMenu != NULL) {
            DeleteMenu(hMenu, SC_CLOSE, MF_BYCOMMAND);
        }
    }
}
