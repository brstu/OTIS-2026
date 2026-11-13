#include <gtest/gtest.h>
#include <limits>
#include <string>
#include "../src/stability_analyzer.h"

struct StabilityCase { double a; bool stable; };
class Model11StabilityParameterizedTest : public ::testing::TestWithParam<StabilityCase> {};
TEST_P(Model11StabilityParameterizedTest, RootMustBeInsideUnitCircle) {
    const auto p = GetParam();
    EXPECT_EQ(StabilityAnalyzer::model11Stable(p.a), p.stable);
}
INSTANTIATE_TEST_SUITE_P(UnitCircleCases, Model11StabilityParameterizedTest,
    ::testing::Values(StabilityCase{0.0, true}, StabilityCase{0.9, true},
                      StabilityCase{-0.99, true}, StabilityCase{1.0, false},
                      StabilityCase{-1.0, false}, StabilityCase{1.01, false},
                      StabilityCase{-1.2, false}));

TEST(Model11StabilityTest, RejectsNotANumberAndInfinity) {
    EXPECT_FALSE(StabilityAnalyzer::model11Stable(std::numeric_limits<double>::quiet_NaN()));
    EXPECT_FALSE(StabilityAnalyzer::model11Stable(std::numeric_limits<double>::infinity()));
}
TEST(Model11StabilityTest, GeneratesWarningForUnstableParameters) {
    EXPECT_NE(StabilityAnalyzer::model11Message(1.2).find("ПРЕДУПРЕЖДЕНИЕ"), std::string::npos);
    EXPECT_NE(StabilityAnalyzer::model11Message(0.5).find("устойчива"), std::string::npos);
}
TEST(Model28StabilityTest, ChecksLocalLinearizationAtZeroEquilibrium) {
    EXPECT_TRUE(StabilityAnalyzer::model28LocallyStable(0.5));
    EXPECT_FALSE(StabilityAnalyzer::model28LocallyStable(1.0));
    EXPECT_FALSE(StabilityAnalyzer::model28LocallyStable(-1.1));
}
TEST(Model36StabilityTest, ZeroEquilibriumIsNotAsymptoticallyStableByLinearization) {
    EXPECT_FALSE(StabilityAnalyzer::model36LocallyAsymptoticallyStable(0.1, 0.5));
    EXPECT_FALSE(StabilityAnalyzer::model36LocallyAsymptoticallyStable(0.0, 0.5));
}
