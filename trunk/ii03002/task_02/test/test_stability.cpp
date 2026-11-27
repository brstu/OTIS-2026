#include <gtest/gtest.h>
#include <sstream>
#include <cmath>
#include <limits>
#include <string>
#include "StabilityAnalyzer.h"

struct StabilityCase { double a1, a2, a3; bool stable; };
class StabilityTest : public ::testing::TestWithParam<StabilityCase> {};

TEST_P(StabilityTest, VerdictMatchesRootLocation) {
    const auto p = GetParam();
    EXPECT_EQ(isStable(p.a1, p.a2, p.a3), p.stable);
    const auto result = analyzeStability(p.a1, p.a2, p.a3);
    for (const auto& root : result.roots) {
        EXPECT_TRUE(std::isfinite(root.real()));
        EXPECT_TRUE(std::isfinite(root.imag()));
    }
}

INSTANTIATE_TEST_SUITE_P(InsideOutsideAndBoundary, StabilityTest, ::testing::Values(
    StabilityCase{0.4, 0.3, 0.1, true},
    StabilityCase{0.2, -0.1, 0.05, true},
    StabilityCase{1.4, 0.2, 0.1, false},
    StabilityCase{0.0, 0.0, 1.0, false},  // roots include z=1
    StabilityCase{0.0, 0.0, -1.0, false}, // roots include z=-1
    StabilityCase{0.0, -1.0, 0.0, false}  // roots at +/-i
), [](const ::testing::TestParamInfo<StabilityCase>& info) {
    return "Case" + std::to_string(info.index) + (info.param.stable ? "_Stable" : "_Unstable");
});

TEST(StabilityAnalyzer, StableModelHasAllRootsInsideUnitCircle) {
    const auto result = analyzeStability(0.4, 0.3, 0.1);
    EXPECT_TRUE(result.is_stable);
    EXPECT_LT(result.max_modulus, 1.0);
}

TEST(StabilityAnalyzer, ReportsWarningForUnstableModel) {
    std::ostringstream out;
    printStabilityReport(1.4, 0.2, 0.1, out);
    EXPECT_NE(out.str().find("WARNING"), std::string::npos);
    EXPECT_NE(out.str().find("UNSTABLE"), std::string::npos);
}

TEST(StabilityAnalyzer, DoesNotReportWarningForStableModel) {
    std::ostringstream out;
    printStabilityReport(0.4, 0.3, 0.1, out);
    EXPECT_EQ(out.str().find("WARNING"), std::string::npos);
}

TEST(StabilityAnalyzer, NonFiniteCoefficientsAreUnstable) {
    EXPECT_FALSE(isStable(std::numeric_limits<double>::quiet_NaN(), 0.0, 0.0));
}
