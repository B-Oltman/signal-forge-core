#ifndef CUSTOM_TEST_LOGGER_H
#define CUSTOM_TEST_LOGGER_H

#include "gtest/gtest.h"
#include "Logger.h"

class CustomTestLogger : public ::testing::TestEventListener {
public:
    void OnTestProgramStart(const ::testing::UnitTest& unit_test) override {
        Logger::Logger().LogMessage("Starting test program", Logger::LogLevel::LOG_INFO);
    }

    void OnTestIterationStart(const ::testing::UnitTest& unit_test, int iteration) override {
        Logger::Logger().LogMessage("Starting iteration " + std::to_string(iteration), Logger::LogLevel::LOG_INFO);
    }

    void OnEnvironmentsSetUpStart(const ::testing::UnitTest& unit_test) override {
        Logger::Logger().LogMessage("Setting up environments", Logger::LogLevel::LOG_INFO);
    }

    void OnEnvironmentsSetUpEnd(const ::testing::UnitTest& unit_test) override {
        Logger::Logger().LogMessage("Environments set up", Logger::LogLevel::LOG_INFO);
    }

    void OnTestCaseStart(const ::testing::TestCase& test_case) override {
        Logger::Logger().LogMessage("Starting test case " + std::string(test_case.name()), Logger::LogLevel::LOG_INFO);
    }

    void OnTestStart(const ::testing::TestInfo& test_info) override {
        Logger::Logger().LogMessage("Starting test " + std::string(test_info.test_case_name()) + "." + test_info.name(), Logger::LogLevel::LOG_INFO);
    }

    void OnTestPartResult(const ::testing::TestPartResult& test_part_result) override {
        if (test_part_result.failed()) {
            Logger::Logger().LogMessage("Test failed: " + std::string(test_part_result.summary()), Logger::LogLevel::LOG_ERROR);
        }
    }

    void OnTestEnd(const ::testing::TestInfo& test_info) override {
        Logger::Logger().LogMessage("Test " + std::string(test_info.test_case_name()) + "." + test_info.name() + " ended", Logger::LogLevel::LOG_INFO);
    }

    void OnTestCaseEnd(const ::testing::TestCase& test_case) override {
        Logger::Logger().LogMessage("Test case " + std::string(test_case.name()) + " ended", Logger::LogLevel::LOG_INFO);
    }

    void OnEnvironmentsTearDownStart(const ::testing::UnitTest& unit_test) override {
        Logger::Logger().LogMessage("Tearing down environments", Logger::LogLevel::LOG_INFO);
    }

    void OnEnvironmentsTearDownEnd(const ::testing::UnitTest& unit_test) override {
        Logger::Logger().LogMessage("Environments torn down", Logger::LogLevel::LOG_INFO);
    }

    void OnTestIterationEnd(const ::testing::UnitTest& unit_test, int iteration) override {
        Logger::Logger().LogMessage("Iteration " + std::to_string(iteration) + " ended", Logger::LogLevel::LOG_INFO);
    }

    void OnTestProgramEnd(const ::testing::UnitTest& unit_test) override {
        Logger::Logger().LogMessage("Test program ended", Logger::LogLevel::LOG_INFO);
    }
};

#endif // CUSTOM_TEST_LOGGER_H
