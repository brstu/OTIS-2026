#include <gtest/gtest.h>

#include <algorithm>
#include <cmath>
#include <complex>
#include <limits>
#include <vector>

#include "StabilityAnalyzer.h"

namespace {

using Complex = std::complex<double>;

// Сортировка корней для детерминированного сравнения
std::vector<Complex> sorted(std::vector<Complex> roots) {
    std::sort(roots.begin(), roots.end(), [](const Complex& l, const Complex& r) {
        if (std::abs(l.real() - r.real()) > 1e-9) {
            return l.real() < r.real();
        }
        return l.imag() < r.imag();
    });
    return roots;
}

} // namespace

// ---------------------------------------------------------------- корни

TEST(FindRoots, LinearPolynomial) {
    const auto roots = stability::findRoots({1.0, -0.5});

    ASSERT_EQ(roots.size(), 1u);
    EXPECT_NEAR(roots[0].real(), 0.5, 1e-12);
    EXPECT_NEAR(roots[0].imag(), 0.0, 1e-12);
}

TEST(FindRoots, LinearPolynomialWithNonUnitLeadingCoefficient) {
    const auto roots = stability::findRoots({2.0, -1.0});

    ASSERT_EQ(roots.size(), 1u);
    EXPECT_NEAR(roots[0].real(), 0.5, 1e-12);
}

TEST(FindRoots, QuadraticRealRoots) {
    // z^2 - 3z + 2 = (z - 1)(z - 2)
    const auto roots = sorted(stability::findRoots({1.0, -3.0, 2.0}));

    ASSERT_EQ(roots.size(), 2u);
    EXPECT_NEAR(roots[0].real(), 1.0, 1e-12);
    EXPECT_NEAR(roots[1].real(), 2.0, 1e-12);
}

TEST(FindRoots, QuadraticPurelyImaginaryRoots) {
    // z^2 + 1 = 0 -> z = +-i, |z| = 1
    const auto roots = stability::findRoots({1.0, 0.0, 1.0});

    ASSERT_EQ(roots.size(), 2u);
    EXPECT_NEAR(std::abs(roots[0]), 1.0, 1e-12);
    EXPECT_NEAR(std::abs(roots[1]), 1.0, 1e-12);
    EXPECT_NEAR(roots[0].real(), 0.0, 1e-12);
}

TEST(FindRoots, QuadraticComplexConjugateRoots) {
    // z^2 - z + 0.5 = 0 -> z = 0.5 +- 0.5i
    const auto roots = sorted(stability::findRoots({1.0, -1.0, 0.5}));

    ASSERT_EQ(roots.size(), 2u);
    EXPECT_NEAR(roots[0].real(), 0.5, 1e-12);
    EXPECT_NEAR(roots[0].imag(), -0.5, 1e-12);
    EXPECT_NEAR(roots[1].real(), 0.5, 1e-12);
    EXPECT_NEAR(roots[1].imag(), 0.5, 1e-12);
}

TEST(FindRoots, CubicRealRootsDurandKerner) {
    // (z - 0.5)(z + 0.3)(z - 0.9) = z^3 - 1.1 z^2 + 0.03 z + 0.135
    const auto roots = sorted(stability::findRoots({1.0, -1.1, 0.03, 0.135}));

    ASSERT_EQ(roots.size(), 3u);
    EXPECT_NEAR(roots[0].real(), -0.3, 1e-8);
    EXPECT_NEAR(roots[1].real(), 0.5, 1e-8);
    EXPECT_NEAR(roots[2].real(), 0.9, 1e-8);
}

TEST(FindRoots, CubicWithComplexRoots) {
    // (z - 1.5)(z^2 + 0.25) = z^3 - 1.5 z^2 + 0.25 z - 0.375
    const auto roots = stability::findRoots({1.0, -1.5, 0.25, -0.375});

    EXPECT_NEAR(stability::spectralRadius(roots), 1.5, 1e-8);
}

TEST(FindRoots, ThrowsOnEmptyPolynomial) {
    EXPECT_THROW(stability::findRoots({}), std::invalid_argument);
}

TEST(FindRoots, ThrowsOnConstantPolynomial) {
    EXPECT_THROW(stability::findRoots({5.0}), std::invalid_argument);
}

TEST(FindRoots, ThrowsOnZeroLeadingCoefficient) {
    EXPECT_THROW(stability::findRoots({0.0, 1.0, 2.0}), std::invalid_argument);
}

TEST(FindRoots, ThrowsOnNonFiniteCoefficient) {
    const double nan = std::numeric_limits<double>::quiet_NaN();
    const double inf = std::numeric_limits<double>::infinity();

    EXPECT_THROW(stability::findRoots({1.0, nan}), std::invalid_argument);
    EXPECT_THROW(stability::findRoots({1.0, inf}), std::invalid_argument);
}

TEST(SpectralRadius, EmptyRootsGiveZero) {
    EXPECT_DOUBLE_EQ(stability::spectralRadius({}), 0.0);
}

// ------------------------------------- параметризованно: z - a = 0

struct LinearStabilityCase {
    double a;
    bool expectedStable;
};

class LinearPoleStability : public ::testing::TestWithParam<LinearStabilityCase> {};

TEST_P(LinearPoleStability, StableIffAbsPoleLessThanOne) {
    const auto param = GetParam();

    EXPECT_EQ(stability::isStable({1.0, -param.a}), param.expectedStable)
        << "a = " << param.a;
}

INSTANTIATE_TEST_SUITE_P(
    ZPlane,
    LinearPoleStability,
    ::testing::Values(
        LinearStabilityCase{-1.5, false},
        LinearStabilityCase{-1.0, false},    // граница |z| = 1
        LinearStabilityCase{-0.99, true},
        LinearStabilityCase{0.0, true},
        LinearStabilityCase{0.5, true},
        LinearStabilityCase{0.99, true},
        LinearStabilityCase{1.0, false},     // граница |z| = 1
        LinearStabilityCase{1.001, false},
        LinearStabilityCase{1.5, false}
    )
);

// ---------------------------------------------------------- устойчивость

TEST(Stability, PurelyImaginaryRootsAreOnBoundaryAndUnstable) {
    EXPECT_FALSE(stability::isStable({1.0, 0.0, 1.0}));
}

TEST(Stability, ComplexRootsInsideUnitCircleAreStable) {
    // |0.5 +- 0.5i| = 0.7071
    EXPECT_TRUE(stability::isStable({1.0, -1.0, 0.5}));
}

TEST(Stability, ComplexRootsOutsideUnitCircleAreUnstable) {
    // z^2 - 2z + 2 = 0 -> z = 1 +- i, |z| = 1.4142
    EXPECT_FALSE(stability::isStable({1.0, -2.0, 2.0}));
}

TEST(Stability, StableCubic) {
    EXPECT_TRUE(stability::isStable({1.0, -1.1, 0.03, 0.135}));
}

TEST(Stability, UnstableCubic) {
    EXPECT_FALSE(stability::isStable({1.0, -1.5, 0.25, -0.375}));
}

TEST(Stability, AnalyzeFillsReport) {
    const auto report = stability::analyze({1.0, -0.8});

    EXPECT_TRUE(report.stable);
    ASSERT_EQ(report.roots.size(), 1u);
    EXPECT_NEAR(report.maxModulus, 0.8, 1e-12);
}

TEST(Stability, ToleranceMovesTheBoundary) {
    // Корень 0.95: при допуске 0.1 граница = 0.9 -> уже неустойчиво
    EXPECT_TRUE(stability::isStable({1.0, -0.95}));
    EXPECT_FALSE(stability::isStable({1.0, -0.95}, 0.1));
}

// ---------------------------------------------------------- предупреждение

TEST(Warning, EmptyForStableSystem) {
    const auto report = stability::analyze({1.0, -0.5});

    EXPECT_TRUE(stability::makeWarning("Some model", report).empty());
}

TEST(Warning, ContainsModelNameAndRadiusForUnstableSystem) {
    const auto report = stability::analyze({1.0, -1.5});
    const std::string message = stability::makeWarning("Some model", report);

    EXPECT_NE(message.find("WARNING"), std::string::npos);
    EXPECT_NE(message.find("Some model"), std::string::npos);
    EXPECT_NE(message.find("1.5000"), std::string::npos);
}
