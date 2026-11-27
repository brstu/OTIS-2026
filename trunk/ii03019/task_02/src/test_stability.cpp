#include <gtest/gtest.h>
#include "StabilityAnalyzer.h"

TEST(StabilityAnalyzerTest, StableCoefficients) {
    EXPECT_TRUE(StabilityAnalyzer::isStable(0.5));
    EXPECT_TRUE(StabilityAnalyzer::isStable(-0.8));
    EXPECT_TRUE(StabilityAnalyzer::isStable(0.0));
}

TEST(StabilityAnalyzerTest, BoundaryValues) {
    EXPECT_FALSE(StabilityAnalyzer::isStable(1.0));
    EXPECT_FALSE(StabilityAnalyzer::isStable(-1.0));
}

TEST(StabilityAnalyzerTest, UnstableCoefficients) {
    EXPECT_FALSE(StabilityAnalyzer::isStable(1.5));
    EXPECT_FALSE(StabilityAnalyzer::isStable(-2.0));
}