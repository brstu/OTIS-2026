#include <gtest/gtest.h>
#include "StabilityAnalyzer.h"

#include <vector>
#include <limits>

class StabilityTest : public ::testing::Test
{
protected:
    StabilityAnalyzer analyzer;
};

TEST_F(StabilityTest, StableProcessTest)
{
    std::vector<double> stableOutput =
    {
        1.0, 0.5, 0.2, 0.1, 0.05, 0.02, 0.01
    };

    EXPECT_TRUE(analyzer.isStable(stableOutput));
}

TEST_F(StabilityTest, UnstableProcessTest)
{
    std::vector<double> unstableOutput =
    {
        1.0, 2.0, 5.0, 12.0, 30.0, 1e7
    };

    EXPECT_FALSE(analyzer.isStable(unstableOutput));
}

TEST_F(StabilityTest, EmptyOrSmallVectorTest)
{
    std::vector<double> emptyVec;
    std::vector<double> singleVec = { 1.0 };

    EXPECT_TRUE(analyzer.isStable(emptyVec));
    EXPECT_TRUE(analyzer.isStable(singleVec));
}

TEST_F(StabilityTest, NegativeLargeValueTest)
{
    std::vector<double> output =
    {
        1.0, 2.0, -1e7
    };

    EXPECT_FALSE(analyzer.isStable(output));
}

TEST_F(StabilityTest, NanValueTest)
{
    std::vector<double> output =
    {
        1.0,
        std::numeric_limits<double>::quiet_NaN()
    };

    EXPECT_FALSE(analyzer.isStable(output));
}

TEST_F(StabilityTest, InfinityValueTest)
{
    std::vector<double> output =
    {
        1.0,
        std::numeric_limits<double>::infinity()
    };

    EXPECT_FALSE(analyzer.isStable(output));
}