// Модуль анализа устойчивости: корни на Z-плоскости, граница |z| = 1, предупреждения.
#include <gtest/gtest.h>

#include <complex>
#include <limits>
#include <string>

#include "StabilityAnalyzer.h"

using stability::Class;

// ---------- корни характеристических уравнений ----------
TEST(StabilityRoots, FirstOrderRootEqualsA) {
    EXPECT_EQ(stability::firstOrderRoot(0.5), std::complex<double>(0.5, 0.0));
    EXPECT_EQ(stability::firstOrderRoot(-1.25), std::complex<double>(-1.25, 0.0));
}

TEST(StabilityRoots, EulerRootIsOneMinusADt) {
    EXPECT_NEAR(stability::eulerRoot(0.8, 0.05).real(), 0.96, 1e-12);
    EXPECT_NEAR(stability::eulerRoot(2.0, 1.0).real(), -1.0, 1e-12);
    EXPECT_DOUBLE_EQ(stability::eulerRoot(0.8, 0.05).imag(), 0.0);
}

// ---------- классификация модуля корня ----------
struct RootCase {
    std::complex<double> z;
    Class expected;
};

class ClassifyRootTest : public ::testing::TestWithParam<RootCase> {};

TEST_P(ClassifyRootTest, ClassifiesByModulus) {
    EXPECT_EQ(stability::classifyRoot(GetParam().z), GetParam().expected);
}

INSTANTIATE_TEST_SUITE_P(
    UnitCircle, ClassifyRootTest,
    ::testing::Values(
        RootCase{{0.0, 0.0}, Class::Stable},
        RootCase{{0.5, 0.0}, Class::Stable},
        RootCase{{0.6, 0.6}, Class::Stable},       // |z| ~ 0.849 - комплексный корень
        RootCase{{1.0, 0.0}, Class::Boundary},
        RootCase{{-1.0, 0.0}, Class::Boundary},
        RootCase{{0.0, 1.0}, Class::Boundary},     // чисто мнимый, |z| = 1
        RootCase{{0.6, 0.8}, Class::Boundary},     // 0.36 + 0.64 = 1
        RootCase{{1.5, 0.0}, Class::Unstable},
        RootCase{{0.8, 0.8}, Class::Unstable},     // |z| ~ 1.131
        RootCase{{std::numeric_limits<double>::quiet_NaN(), 0.0}, Class::Invalid},
        RootCase{{0.0, std::numeric_limits<double>::infinity()}, Class::Invalid}));

// ---------- модель 1.7 / 2.9: z = a ----------
struct FirstOrderCase {
    double a;
    Class expected;
};

class FirstOrderStabilityTest : public ::testing::TestWithParam<FirstOrderCase> {};

TEST_P(FirstOrderStabilityTest, MatchesUnitCircleCriterion) {
    const auto& p = GetParam();
    EXPECT_EQ(stability::classifyFirstOrder(p.a), p.expected);
    EXPECT_EQ(stability::isStable(stability::classifyFirstOrder(p.a)), p.expected == Class::Stable);
}

INSTANTIATE_TEST_SUITE_P(
    Coefficients, FirstOrderStabilityTest,
    ::testing::Values(
        FirstOrderCase{0.0, Class::Stable},
        FirstOrderCase{0.5, Class::Stable},
        FirstOrderCase{-0.5, Class::Stable},
        FirstOrderCase{0.999999, Class::Stable},
        FirstOrderCase{-0.999999, Class::Stable},
        FirstOrderCase{1.0, Class::Boundary},
        FirstOrderCase{-1.0, Class::Boundary},
        FirstOrderCase{1.000001, Class::Unstable},
        FirstOrderCase{-1.000001, Class::Unstable},
        FirstOrderCase{1.5, Class::Unstable},
        FirstOrderCase{-2.0, Class::Unstable},
        FirstOrderCase{std::numeric_limits<double>::quiet_NaN(), Class::Invalid},
        FirstOrderCase{std::numeric_limits<double>::infinity(), Class::Invalid}));

// ---------- схема Эйлера: z = 1 - a*dt ----------
struct EulerCase {
    double a;
    double dt;
    Class expected;
};

class EulerStabilityTest : public ::testing::TestWithParam<EulerCase> {};

TEST_P(EulerStabilityTest, MatchesZeroToTwoCriterion) {
    const auto& p = GetParam();
    EXPECT_EQ(stability::classifyEuler(p.a, p.dt), p.expected);
}

INSTANTIATE_TEST_SUITE_P(
    Steps, EulerStabilityTest,
    ::testing::Values(
        EulerCase{0.8, 0.05, Class::Stable},    // a*dt = 0.04
        EulerCase{1.0, 1.0, Class::Stable},     // z = 0 (мгновенное затухание)
        EulerCase{1.0, 1.9, Class::Stable},     // z = -0.9
        EulerCase{1.0, 2.0, Class::Boundary},   // z = -1
        EulerCase{1.0, 2.1, Class::Unstable},   // z = -1.1
        EulerCase{0.0, 0.1, Class::Boundary},   // z = 1: затухания нет
        EulerCase{-1.0, 0.1, Class::Unstable},  // z = 1.1: растёт сам объект
        EulerCase{1.0, 0.0, Class::Invalid},
        EulerCase{1.0, -0.1, Class::Invalid},
        EulerCase{std::numeric_limits<double>::quiet_NaN(), 0.1, Class::Invalid},
        EulerCase{1.0, std::numeric_limits<double>::infinity(), Class::Invalid}));

// ---------- тексты предупреждений ----------
TEST(StabilityMessages, StableSystemsGiveEmptyWarning) {
    EXPECT_EQ(stability::firstOrderWarning("M", 0.5), "");
    EXPECT_EQ(stability::eulerWarning("M", 0.8, 0.05), "");
}

TEST(StabilityMessages, FirstOrderWarningsDistinguishCases) {
    const std::string boundary = stability::firstOrderWarning("Model X", 1.0);
    const std::string unstable = stability::firstOrderWarning("Model X", 1.5);
    const std::string invalid =
        stability::firstOrderWarning("Model X", std::numeric_limits<double>::quiet_NaN());

    EXPECT_NE(boundary.find("boundary"), std::string::npos);
    EXPECT_NE(boundary.find("Model X"), std::string::npos);
    EXPECT_NE(unstable.find("UNSTABLE"), std::string::npos);
    EXPECT_NE(invalid.find("finite"), std::string::npos);
    EXPECT_NE(boundary, unstable);
}

TEST(StabilityMessages, EulerWarningsDistinguishCases) {
    EXPECT_NE(stability::eulerWarning("M", 1.0, 2.0).find("boundary"), std::string::npos);
    EXPECT_NE(stability::eulerWarning("M", 1.0, 2.5).find("numerically unstable"), std::string::npos);
    EXPECT_NE(stability::eulerWarning("M", -1.0, 0.1).find("object itself"), std::string::npos);
    EXPECT_NE(stability::eulerWarning("M", 1.0, 0.0).find("dt"), std::string::npos);
}
