#include <gtest/gtest.h>
#include <cmath>
#include <sstream>
#include "StabilityAnalyzer.h"

TEST(StabilityAnalyzerTest, StableCoefficientsReturnTrue) {
    const auto report = StabilityAnalyzer::analyzeModel1_6(0.3, 0.2, 0.1);
    EXPECT_TRUE(report.isStable());
    EXPECT_EQ(report.status, StabilityStatus::STABLE);
    EXPECT_LT(report.max_root_modulus, 1.0);
    EXPECT_TRUE(report.warning_message.empty());
}

TEST(StabilityAnalyzerTest, BoundaryRootOnUnitCircle) {
    const auto report = StabilityAnalyzer::analyzeModel1_6(1.0, 0.0, 0.0);
    EXPECT_FALSE(report.isStable());
    EXPECT_EQ(report.status, StabilityStatus::MARGINALLY_STABLE);
    EXPECT_NEAR(report.max_root_modulus, 1.0, 1e-6);

    std::ostringstream oss;
    const bool ok = StabilityAnalyzer::checkAndWarnIfUnstable(report, oss);
    EXPECT_FALSE(ok);
    EXPECT_NE(oss.str().find("WARNING"), std::string::npos);
}

TEST(StabilityAnalyzerTest, UnstableCoefficientsEmitWarning) {
    const auto report = StabilityAnalyzer::analyzeModel1_6(1.2, 0.3, 0.1);
    EXPECT_FALSE(report.isStable());
    EXPECT_EQ(report.status, StabilityStatus::UNSTABLE);
    EXPECT_GT(report.max_root_modulus, 1.0);

    std::ostringstream oss;
    const bool ok = StabilityAnalyzer::checkAndWarnIfUnstable(report, oss);
    EXPECT_FALSE(ok);
    EXPECT_NE(oss.str().find("UNSTABLE"), std::string::npos);
}

TEST(StabilityAnalyzerTest, ComplexAndMultipleRootsBranches) {
    const auto cubic_real_complex = StabilityAnalyzer::analyzeModel1_6(0.2, 0.1, 0.05);
    EXPECT_TRUE(cubic_real_complex.isStable());

    const auto three_real = StabilityAnalyzer::analyzeModel1_6(0.2, -0.4, 0.1);
    EXPECT_TRUE(three_real.isStable());

    const auto zero_rep = StabilityAnalyzer::analyzeModel1_6(0.0, 0.0, 0.0);
    EXPECT_TRUE(zero_rep.isStable());

    const auto double_rep = StabilityAnalyzer::analyzeModel1_6(0.0, 0.25, 0.0);
    EXPECT_TRUE(double_rep.isStable());

    const auto boundary_cubic = StabilityAnalyzer::analyzeModel1_6(0.0, 1.0, 0.0);
    EXPECT_EQ(boundary_cubic.status, StabilityStatus::MARGINALLY_STABLE);

    const auto unstable_cubic = StabilityAnalyzer::analyzeModel1_6(0.0, 1.5, 0.0);
    EXPECT_EQ(unstable_cubic.status, StabilityStatus::UNSTABLE);

    const auto negative_a1 = StabilityAnalyzer::analyzeModel1_6(-0.5, -0.2, -0.1);
    EXPECT_TRUE(negative_a1.isStable());

    const auto shifted_real = StabilityAnalyzer::findCubicRoots(-0.3, -0.03, 0.01);
    EXPECT_EQ(shifted_real.size(), 3u);

    const auto near_disc_zero = StabilityAnalyzer::findCubicRoots(0.0, 0.0, 1e-13);
    EXPECT_EQ(near_disc_zero.size(), 3u);

    std::ostringstream oss;
    EXPECT_TRUE(StabilityAnalyzer::checkAndWarnIfUnstable(cubic_real_complex, oss));
    EXPECT_TRUE(oss.str().empty());
}

TEST(StabilityAnalyzerTest, Model3_7EulerStabilityCheck) {
    const auto stable_rep = StabilityAnalyzer::analyzeModel3_7(0.0, 0.5);
    EXPECT_TRUE(stable_rep.isStable());

    const auto boundary_rep = StabilityAnalyzer::analyzeModel3_7(0.0, 2.0);
    EXPECT_FALSE(boundary_rep.isStable());
    EXPECT_EQ(boundary_rep.status, StabilityStatus::MARGINALLY_STABLE);

    std::ostringstream oss;
    EXPECT_FALSE(StabilityAnalyzer::checkAndWarnIfUnstable(boundary_rep, oss));
    EXPECT_NE(oss.str().find("WARNING"), std::string::npos);

    const auto unstable_rep = StabilityAnalyzer::analyzeModel3_7(0.0, 3.0);
    EXPECT_FALSE(unstable_rep.isStable());
    EXPECT_EQ(unstable_rep.status, StabilityStatus::UNSTABLE);
}