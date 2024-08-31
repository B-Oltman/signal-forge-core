#ifndef SIGNAL_CONTAINER_H
#define SIGNAL_CONTAINER_H

#include "CommonTypes.h"
#include <unordered_map>
#include <queue>
#include <optional>
#include <memory>
#include <mutex>

class SignalContainer {
public:
    static SignalContainer& Instance() {
        static SignalContainer instance;
        return instance;
    }

    void AddSignal(const TradeSignal& signal) {
        std::lock_guard<std::mutex> lock(mtx_);
        unprocessedSignals[signal.signalKey].push(signal);
        signalsById[signal.id] = signal;
    }

    std::optional<TradeSignal> GetSignal(const std::string& key) {
        std::lock_guard<std::mutex> lock(mtx_);
        if (unprocessedSignals.find(key) != unprocessedSignals.end() && !unprocessedSignals[key].empty()) {
            TradeSignal signal = unprocessedSignals[key].front();
            unprocessedSignals[key].pop();
            signalsById.erase(signal.id);
            return signal;
        }
        return std::nullopt;
    }

    std::optional<TradeSignal> GetSignalById(const std::string& id) const {
        std::lock_guard<std::mutex> lock(mtx_);
        if (signalsById.find(id) != signalsById.end()) {
            return signalsById.at(id);
        }
        return std::nullopt;
    }

    bool RemoveSignalById(const std::string& id) {
        std::lock_guard<std::mutex> lock(mtx_);
        if (signalsById.find(id) != signalsById.end()) {
            TradeSignal signal = signalsById[id];
            signalsById.erase(id);
            auto& queue = unprocessedSignals[signal.signalKey];
            std::queue<TradeSignal> tempQueue;
            while (!queue.empty()) {
                TradeSignal tempSignal = queue.front();
                queue.pop();
                if (tempSignal.id != id) {
                    tempQueue.push(tempSignal);
                }
            }
            std::swap(queue, tempQueue);
            return true;
        }
        return false;
    }

    bool RemoveSignal(const std::string& key) {
        std::lock_guard<std::mutex> lock(mtx_);
        if (unprocessedSignals.find(key) != unprocessedSignals.end() && !unprocessedSignals[key].empty()) {
            TradeSignal signal = unprocessedSignals[key].front();
            unprocessedSignals[key].pop();
            signalsById.erase(signal.id);
            return true;
        }
        return false;
    }

    void RemoveAllSignals(const std::string& key) {
        std::lock_guard<std::mutex> lock(mtx_);
        if (unprocessedSignals.find(key) != unprocessedSignals.end()) {
            auto& queue = unprocessedSignals[key];
            while (!queue.empty()) {
                signalsById.erase(queue.front().id);
                queue.pop();
            }
            unprocessedSignals.erase(key);
        }
    }

private:
    SignalContainer() = default;
    ~SignalContainer() = default;

    SignalContainer(const SignalContainer&) = delete;
    SignalContainer& operator=(const SignalContainer&) = delete;

    mutable std::mutex mtx_;
    std::unordered_map<std::string, std::queue<TradeSignal>> unprocessedSignals;
    std::unordered_map<std::string, TradeSignal> signalsById;
};

#endif // SIGNAL_CONTAINER_H
