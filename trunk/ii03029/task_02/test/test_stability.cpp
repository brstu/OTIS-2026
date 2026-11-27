#include <gtest/gtest.h>
#include "../src/StabilityAnalyzer.h"

TEST(StabilityAnalyzerTest, ValidStableCoefficients) {
    EXPECT_TRUE(StabilityAnalyzer::isStableLinear2ndOrder(0.5, 0.2));
}

TEST(StabilityAnalyzerTest, UnstableCoefficients) {
    EXPECT_FALSE(StabilityAnalyzer::isStableLinear2ndOrder(1.5, 0.2));
}