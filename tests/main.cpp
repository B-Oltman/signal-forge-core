#include "gtest/gtest.h"
#include "CustomTestLogger.h"

int main(int argc, char **argv) {
    std::cout << "Starting tests..." << std::endl;
    ::testing::InitGoogleTest(&argc, argv);

    // Remove the default listener
    ::testing::TestEventListeners& listeners = ::testing::UnitTest::GetInstance()->listeners();
    delete listeners.Release(listeners.default_result_printer());

    // Add custom listener
    listeners.Append(new CustomTestLogger());

    return RUN_ALL_TESTS();
}
