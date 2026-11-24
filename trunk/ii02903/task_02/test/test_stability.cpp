// Tests of the stability analyzer: roots of z^2 - a1*z - a2 = 0 must lie inside the unit circle.
#include <cmath>
#include <string>
#include <tuple>

#include <gtest/gtest.h>

#include "models.h"
#include "stability.h"

// ---------- roots and verdict for typical parameters ----------
// z^2 - 0.5 z - 0.3 = 0:  D = 0.25 + 1.2 = 1.45,  z = (0.5 +- 1.20416)/2 = 0.85208 and -0.35208
TEST(StabilityAnalyzer, StableSystemWithRealRoots) {
    const StabilityResult r = StabilityAnalyzer::analyze(0.5, 0.3);
    EXPECT_TRUE(r.stable);
    EXPECT_NEAR(r.re1, 0.5 * (0.5 + std::sqrt(1.45)), 1e-12);
    EXPECT_NEAR(r.re2, 0.5 * (0.5 - std::sqrt(1.45)), 1e-12);
    EXPECT_DOUBLE_EQ(r.im1, 0.0);
    EXPECT_DOUBLE_EQ(r.im2, 0.0);
    EXPECT_NEAR(r.maxModulus, 0.852079728939617, 1e-12);
}

// z^2 - 1.2 z - 0.5 = 0:  D = 1.44 + 2 = 3.44,  z1 = 1.52713 > 1  -> diverges
TEST(StabilityAnalyzer, UnstableSystemIsDetected) {
    const StabilityResult r = StabilityAnalyzer::analyze(1.2, 0.5);
    EXPECT_FALSE(r.stable);
    EXPECT_NEAR(r.maxModulus, 0.5 * (1.2 + std::sqrt(3.44)), 1e-12);
    EXPECT_GT(r.maxModulus, 1.0);
}

// z^2 - 0.2 z + 0.5 = 0:  D = 0.04 - 2 = -1.96,  z = 0.1 +- 0.7i,  |z| = sqrt(0.5) = 0.707
TEST(StabilityAnalyzer, ComplexRootsInsideUnitCircleAreStable) {
    const StabilityResult r = StabilityAnalyzer::analyze(0.2, -0.5);
    EXPECT_TRUE(r.stable);
    EXPECT_NEAR(r.re1, 0.1, 1e-12);
    EXPECT_NEAR(r.im1, 0.7, 1e-12);
    EXPECT_NEAR(r.im2, -0.7, 1e-12);
    EXPECT_NEAR(r.maxModulus, std::sqrt(0.5), 1e-12);
}

// z^2 - 0.2 z + 1.5 = 0 (written as a2 = -1.5): |z| = sqrt(1.5) = 1.22 > 1
TEST(StabilityAnalyzer, ComplexRootsOutsideUnitCircleAreUnstable) {
    const StabilityResult r = StabilityAnalyzer::analyze(0.2, -1.5);
    EXPECT_FALSE(r.stable);
    EXPECT_NEAR(r.maxModulus, std::sqrt(1.5), 1e-12);
}

TEST(StabilityAnalyzer, ZeroCoefficientsGiveZeroRoots) {
    const StabilityResult r = StabilityAnalyzer::analyze(0.0, 0.0);
    EXPECT_TRUE(r.stable);
    EXPECT_DOUBLE_EQ(r.maxModulus, 0.0);
}

// ---------- 2. boundary values of the Z-plane: |z| = 1 ----------
// A root exactly on the unit circle means the process does not decay, so it is NOT stable.
struct BoundaryCase {
    double a1, a2;
    const char* roots;
};

class StabilityBoundaryTest : public ::testing::TestWithParam<BoundaryCase> {};

TEST_P(StabilityBoundaryTest, RootOnUnitCircleIsNotStable) {
    const BoundaryCase c = GetParam();
    const StabilityResult r = StabilityAnalyzer::analyze(c.a1, c.a2);
    EXPECT_NEAR(r.maxModulus, 1.0, 1e-12) << "roots: " << c.roots;
    EXPECT_FALSE(r.stable) << "roots: " << c.roots;
    EXPECT_FALSE(StabilityAnalyzer::isStableJury(c.a1, c.a2)) << "roots: " << c.roots;
}

INSTANTIATE_TEST_SUITE_P(
    OnTheCircle, StabilityBoundaryTest,
    ::testing::Values(BoundaryCase{1.0, 0.0, "1 and 0"},
                      BoundaryCase{-1.0, 0.0, "-1 and 0"},
                      BoundaryCase{0.0, 1.0, "1 and -1"},
                      BoundaryCase{1.5, -0.5, "1 and 0.5"},
                      BoundaryCase{0.0, -1.0, "+i and -i"}));

TEST(StabilityAnalyzer, JustInsideTheCircleIsStableJustOutsideIsNot) {
    EXPECT_TRUE(StabilityAnalyzer::analyze(0.99, 0.0).stable);
    EXPECT_TRUE(StabilityAnalyzer::analyze(0.0, 0.99).stable);
    EXPECT_TRUE(StabilityAnalyzer::analyze(0.0, -0.99).stable);
    EXPECT_FALSE(StabilityAnalyzer::analyze(1.01, 0.0).stable);
    EXPECT_FALSE(StabilityAnalyzer::analyze(0.0, 1.01).stable);
    EXPECT_FALSE(StabilityAnalyzer::analyze(0.0, -1.01).stable);
}

// ---------- parameterized: the roots method and the Jury criterion must agree ----------
class StabilityVsJuryTest : public ::testing::TestWithParam<std::tuple<double, double>> {};

TEST_P(StabilityVsJuryTest, BothMethodsGiveTheSameVerdict) {
    const double a1 = std::get<0>(GetParam());
    const double a2 = std::get<1>(GetParam());
    EXPECT_EQ(StabilityAnalyzer::analyze(a1, a2).stable, StabilityAnalyzer::isStableJury(a1, a2))
        << "a1 = " << a1 << ", a2 = " << a2;
}

INSTANTIATE_TEST_SUITE_P(Grid, StabilityVsJuryTest,
                         ::testing::Combine(::testing::Values(-2.1, -1.3, -0.7, 0.0, 0.45, 1.15, 1.9),
                                            ::testing::Values(-1.4, -0.6, -0.12, 0.2, 0.8, 1.2)));

// ---------- text for the console ----------
TEST(StabilityAnalyzer, DescribesRealRoots) {
    const std::string text = StabilityAnalyzer::describe(StabilityAnalyzer::analyze(0.5, 0.3));
    EXPECT_NE(text.find("z1 ="), std::string::npos);
    EXPECT_NE(text.find("z2 ="), std::string::npos);
    EXPECT_NE(text.find("max|z|"), std::string::npos);
}

TEST(StabilityAnalyzer, DescribesComplexRoots) {
    const std::string text = StabilityAnalyzer::describe(StabilityAnalyzer::analyze(0.2, -0.5));
    EXPECT_NE(text.find("+/-"), std::string::npos);
    EXPECT_NE(text.find("i"), std::string::npos);
    EXPECT_NE(text.find("max|z|"), std::string::npos);
}

// ---------- the warning before the simulation ----------
TEST(StabilityReport, UnstableSystemProducesWarning) {
    const std::string text = StabilityAnalyzer::report(false, "roots...");
    EXPECT_NE(text.find("WARNING"), std::string::npos);
    EXPECT_NE(text.find("NOT stable"), std::string::npos);
    EXPECT_NE(text.find("roots..."), std::string::npos);
}

TEST(StabilityReport, StableSystemProducesNoWarning) {
    const std::string text = StabilityAnalyzer::report(true, "roots...");
    EXPECT_EQ(text.find("WARNING"), std::string::npos);
    EXPECT_NE(text.find("The system is stable"), std::string::npos);
}

TEST(StabilityReport, NothingToReportWithoutInformation) {
    EXPECT_TRUE(StabilityAnalyzer::report(true, "").empty());
    EXPECT_TRUE(StabilityAnalyzer::report(false, "").empty());
}

// The whole chain: the user enters coefficients -> the model is checked -> the warning appears.
TEST(StabilityReport, WarningIsShownForDivergingCoefficientsOfModel13) {
    std::string info;
    const LinearModel13 bad(1.2, 0.5, 1.0, 0.0);
    const bool stable = bad.isStable(info);
    EXPECT_FALSE(stable);
    EXPECT_NE(StabilityAnalyzer::report(stable, info).find("WARNING"), std::string::npos);

    const LinearModel13 good(0.5, 0.3, 1.0, 0.0);
    const bool goodStable = good.isStable(info);
    EXPECT_TRUE(goodStable);
    EXPECT_EQ(StabilityAnalyzer::report(goodStable, info).find("WARNING"), std::string::npos);
}
