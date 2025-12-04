/**
 * @brief Test suite for the libatmega library.
 */
#include <iostream>

#include <gtest/gtest.h>

/**
 * @brief Run test suite.
 * 
 * @return The number of failed tests.
 */
int main(int argc, char **argv) 
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}