#include "gtest/gtest.h"

namespace tests::unit_tests
{
    int main(int argc, char** argv)
    {
        ::testing::InitGoogleTest(&argc, argv);
        return RUN_ALL_TESTS();
    }
} // namespace tests::unit_tests