// Тесты модуля анализа устойчивости (Z-плоскость, корень z = a модели 1.1).
#include <gtest/gtest.h>

#include <cmath>
#include <limits>
#include <sstream>
#include <stdexcept>
#include <utility>

#include "StabilityAnalyzer.h"

TEST(Stability, RootOfCharacteristicEquationEqualsA) {
    EXPECT_DOUBLE_EQ(StabilityAnalyzer::characteristicRoot(0.3), 0.3);
    EXPECT_DOUBLE_EQ(StabilityAnalyzer::characteristicRoot(-2.0), -2.0);
}

TEST(Stability, StableForSmallCoefficient) {
    EXPECT_TRUE(StabilityAnalyzer::isStable(0.5));
    EXPECT_TRUE(StabilityAnalyzer::isStable(-0.5));
    EXPECT_TRUE(StabilityAnalyzer::isStable(0.0));
}

TEST(Stability, UnstableForLargeCoefficient) {
    EXPECT_FALSE(StabilityAnalyzer::isStable(1.5));
    EXPECT_FALSE(StabilityAnalyzer::isStable(-1.5));
}

// Граница Z-плоскости |z| = 1
TEST(Stability, UnitCircleBoundaryIsNotStable) {
    EXPECT_FALSE(StabilityAnalyzer::isStable(1.0));
    EXPECT_FALSE(StabilityAnalyzer::isStable(-1.0));
    EXPECT_EQ(StabilityAnalyzer::classify(1.0), Stability::Marginal);
    EXPECT_EQ(StabilityAnalyzer::classify(-1.0), Stability::Marginal);
}

TEST(Stability, JustInsideAndJustOutsideOfUnitCircle) {
    EXPECT_EQ(StabilityAnalyzer::classify(0.999), Stability::Stable);
    EXPECT_EQ(StabilityAnalyzer::classify(-0.999), Stability::Stable);
    EXPECT_EQ(StabilityAnalyzer::classify(1.001), Stability::Unstable);
    EXPECT_EQ(StabilityAnalyzer::classify(-1.001), Stability::Unstable);
}

TEST(Stability, ToleranceAroundBoundary) {
    EXPECT_EQ(StabilityAnalyzer::classify(1.0 + 1e-12), Stability::Marginal);
    EXPECT_EQ(StabilityAnalyzer::classify(1.0 - 1e-12), Stability::Marginal);
    EXPECT_EQ(StabilityAnalyzer::classify(0.99, 0.05), Stability::Marginal);   // свой eps
}

TEST(Stability, NonFiniteCoefficientIsUnstable) {
    EXPECT_EQ(StabilityAnalyzer::classify(std::numeric_limits<double>::quiet_NaN()), Stability::Unstable);
    EXPECT_EQ(StabilityAnalyzer::classify(std::numeric_limits<double>::infinity()), Stability::Unstable);
}

// ---------- Предупреждения и исключения ----------

TEST(StabilityWarning, EmptyForStableSystem) {
    EXPECT_TRUE(StabilityAnalyzer::warningMessage(0.5).empty());
    std::ostringstream out;
    EXPECT_TRUE(StabilityAnalyzer::warnIfUnstable(0.5, out));
    EXPECT_TRUE(out.str().empty());
}

TEST(StabilityWarning, PrintedForUnstableSystem) {
    std::ostringstream out;
    EXPECT_FALSE(StabilityAnalyzer::warnIfUnstable(1.5, out));
    EXPECT_NE(out.str().find("ПРЕДУПРЕЖДЕНИЕ"), std::string::npos);
    EXPECT_NE(out.str().find("неустойчива"), std::string::npos);
}

TEST(StabilityWarning, PrintedForMarginalSystem) {
    std::ostringstream out;
    EXPECT_FALSE(StabilityAnalyzer::warnIfUnstable(1.0, out));
    EXPECT_NE(out.str().find("на единичной окружности"), std::string::npos);
}

TEST(StabilityWarning, RequireStableThrowsForUnstable) {
    EXPECT_THROW(StabilityAnalyzer::requireStable(2.0), std::domain_error);
    EXPECT_THROW(StabilityAnalyzer::requireStable(-1.0), std::domain_error);
    EXPECT_NO_THROW(StabilityAnalyzer::requireStable(0.9));
}

// ---------- TEST_P: единая проверка согласованности классификации и isStable ----------

using StabilityCase = std::pair<double, Stability>;
class StabilityClassification : public ::testing::TestWithParam<StabilityCase> {};

TEST_P(StabilityClassification, ClassifyMatchesExpectation) {
    const auto [a, expected] = GetParam();
    EXPECT_EQ(StabilityAnalyzer::classify(a), expected);
    EXPECT_EQ(StabilityAnalyzer::isStable(a), expected == Stability::Stable);
    EXPECT_EQ(StabilityAnalyzer::warningMessage(a).empty(), expected == Stability::Stable);
}

INSTANTIATE_TEST_SUITE_P(Table, StabilityClassification,
    ::testing::Values(
        StabilityCase{0.0, Stability::Stable}, StabilityCase{0.99, Stability::Stable},
        StabilityCase{-0.99, Stability::Stable}, StabilityCase{1.0, Stability::Marginal},
        StabilityCase{-1.0, Stability::Marginal}, StabilityCase{1.01, Stability::Unstable},
        StabilityCase{-10.0, Stability::Unstable}));
