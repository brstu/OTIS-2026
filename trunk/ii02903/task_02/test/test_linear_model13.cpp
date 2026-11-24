// Tests of Model 1.3:  y(tau+1) = a1*y(tau) + a2*y(tau-1) + b*u(tau)
#include <algorithm>
#include <cmath>
#include <vector>

#include <gtest/gtest.h>

#include "models.h"
#include "stability.h"

namespace {

// Runs the model with the given inputs and returns y(1), y(2), ...
std::vector<double> run(LinearModel13& m, const std::vector<double>& inputs) {
    std::vector<double> y;
    for (double u : inputs) y.push_back(m.nextStep(u));
    return y;
}

}  // namespace

// ---------- 1. values calculated by hand ----------
// a1 = 0.5, a2 = 0.3, b = 1, y(0) = 0, y(-1) = 0, step u = 1:
//   y1 = 0.5*0   + 0.3*0   + 1 = 1
//   y2 = 0.5*1   + 0.3*0   + 1 = 1.5
//   y3 = 0.5*1.5 + 0.3*1   + 1 = 2.05
//   y4 = 0.5*2.05+ 0.3*1.5 + 1 = 2.475
TEST(LinearModel13, StepResponseMatchesHandCalculation) {
    LinearModel13 m(0.5, 0.3, 1.0, 0.0);
    const std::vector<double> y = run(m, {1, 1, 1, 1, 1, 1});
    EXPECT_NEAR(y[0], 1.0, 1e-12);
    EXPECT_NEAR(y[1], 1.5, 1e-12);
    EXPECT_NEAR(y[2], 2.05, 1e-12);
    EXPECT_NEAR(y[3], 2.475, 1e-12);
    EXPECT_NEAR(y[4], 2.8525, 1e-12);
    EXPECT_NEAR(y[5], 3.16875, 1e-12);
}

// Impulse u = 1, 0, 0, 0:  y1 = 1, y2 = 0.5, y3 = 0.25 + 0.3 = 0.55, y4 = 0.275 + 0.15 = 0.425
TEST(LinearModel13, ImpulseResponseMatchesHandCalculation) {
    LinearModel13 m(0.5, 0.3, 1.0, 0.0);
    const std::vector<double> y = run(m, {1, 0, 0, 0});
    EXPECT_NEAR(y[0], 1.0, 1e-12);
    EXPECT_NEAR(y[1], 0.5, 1e-12);
    EXPECT_NEAR(y[2], 0.55, 1e-12);
    EXPECT_NEAR(y[3], 0.425, 1e-12);
}

// Free motion from y(0) = 2 with u = 0 (y(-1) = 0):  y1 = 1, y2 = 0.5 + 0.6 = 1.1, y3 = 0.55 + 0.3 = 0.85
TEST(LinearModel13, FreeMotionFromNonZeroInitialValue) {
    LinearModel13 m(0.5, 0.3, 1.0, 2.0);
    EXPECT_DOUBLE_EQ(m.current(), 2.0);
    const std::vector<double> y = run(m, {0, 0, 0});
    EXPECT_NEAR(y[0], 1.0, 1e-12);
    EXPECT_NEAR(y[1], 1.1, 1e-12);
    EXPECT_NEAR(y[2], 0.85, 1e-12);
}

TEST(LinearModel13, CurrentReturnsLastComputedValue) {
    LinearModel13 m(0.5, 0.3, 1.0, 0.0);
    EXPECT_DOUBLE_EQ(m.current(), 0.0);
    const double y1 = m.nextStep(1.0);
    EXPECT_DOUBLE_EQ(m.current(), y1);
}

// ---------- 4. invariants ----------
TEST(LinearModel13, ZeroInputAndZeroStateStayAtRest) {
    LinearModel13 m(0.9, -0.2, 0.5, 0.0);
    for (int i = 0; i < 50; ++i) EXPECT_DOUBLE_EQ(m.nextStep(0.0), 0.0);
}

// The model is linear: the response to 2*u is two times the response to u.
TEST(LinearModel13, ResponseIsProportionalToInput) {
    const std::vector<double> u = {1, -2, 0.5, 3, 0, 1};
    std::vector<double> u2;
    for (double v : u) u2.push_back(2.0 * v);

    LinearModel13 m1(0.9, -0.2, 0.5, 0.0);
    LinearModel13 m2(0.9, -0.2, 0.5, 0.0);
    const std::vector<double> y1 = run(m1, u);
    const std::vector<double> y2 = run(m2, u2);
    for (size_t i = 0; i < u.size(); ++i) EXPECT_NEAR(y2[i], 2.0 * y1[i], 1e-12);
}

// ... and additive: the response to u1 + u2 is the sum of the responses (superposition).
TEST(LinearModel13, SuperpositionHolds) {
    const std::vector<double> ua = {1, 0, 0, 2, 1};
    const std::vector<double> ub = {0, 3, -1, 1, 0};
    std::vector<double> sum;
    for (size_t i = 0; i < ua.size(); ++i) sum.push_back(ua[i] + ub[i]);

    LinearModel13 a(0.4, 0.3, 1.0, 0.0);
    LinearModel13 b(0.4, 0.3, 1.0, 0.0);
    LinearModel13 c(0.4, 0.3, 1.0, 0.0);
    const std::vector<double> ya = run(a, ua);
    const std::vector<double> yb = run(b, ub);
    const std::vector<double> yc = run(c, sum);
    for (size_t i = 0; i < sum.size(); ++i) EXPECT_NEAR(yc[i], ya[i] + yb[i], 1e-12);
}

// ---------- parameterized: convergence of stable systems ----------
// Steady state of a stable system under a step u:  y = b*u / (1 - a1 - a2)
struct ConvergenceCase {
    double a1, a2, b, u;
};

class LinearConvergenceTest : public ::testing::TestWithParam<ConvergenceCase> {};

TEST_P(LinearConvergenceTest, StableSystemConvergesToSteadyState) {
    const ConvergenceCase c = GetParam();
    ASSERT_TRUE(StabilityAnalyzer::analyze(c.a1, c.a2).stable) << "the test case must be stable";

    LinearModel13 m(c.a1, c.a2, c.b, 0.0);
    for (int i = 0; i < 2000; ++i) m.nextStep(c.u);

    const double expected = c.b * c.u / (1.0 - c.a1 - c.a2);
    EXPECT_NEAR(m.current(), expected, 1e-6);
}

INSTANTIATE_TEST_SUITE_P(
    StableSets, LinearConvergenceTest,
    ::testing::Values(ConvergenceCase{0.5, 0.3, 1.0, 1.0},    // roots 0.85 and -0.35
                      ConvergenceCase{0.9, -0.2, 0.5, 1.0},   // default parameters of the program
                      ConvergenceCase{0.0, 0.5, 2.0, -1.0},   // roots +-0.71
                      ConvergenceCase{-0.5, 0.2, 1.0, 3.0},   // negative a1
                      ConvergenceCase{0.2, -0.5, 1.0, 1.0})); // complex roots, |z| = 0.71

// ---------- parameterized: divergence of unstable systems ----------
class LinearDivergenceTest : public ::testing::TestWithParam<ConvergenceCase> {};

TEST_P(LinearDivergenceTest, UnstableSystemGrowsWithoutLimit) {
    const ConvergenceCase c = GetParam();
    ASSERT_FALSE(StabilityAnalyzer::analyze(c.a1, c.a2).stable) << "the test case must be unstable";

    LinearModel13 m(c.a1, c.a2, c.b, 0.0);
    double maxAbs = 0.0;
    for (int i = 0; i < 300; ++i) maxAbs = std::max(maxAbs, std::fabs(m.nextStep(c.u)));
    EXPECT_GT(maxAbs, 1e3);
}

INSTANTIATE_TEST_SUITE_P(
    UnstableSets, LinearDivergenceTest,
    ::testing::Values(ConvergenceCase{1.2, 0.5, 1.0, 1.0},     // root 1.53
                      ConvergenceCase{0.0, 1.5, 1.0, 1.0},     // roots +-1.22
                      ConvergenceCase{-1.2, 0.5, 1.0, 1.0},    // root -1.53
                      ConvergenceCase{0.0, -1.5, 1.0, 1.0}));  // complex roots, |z| = 1.22

// ---------- information about the model ----------
TEST(LinearModel13, DescribesItself) {
    const LinearModel13 m(0.5, 0.3, 1.0, 0.0);
    EXPECT_EQ(m.tag(), "model13");
    EXPECT_NE(m.name().find("1.3"), std::string::npos);
    EXPECT_EQ(m.params(), "a1=0.5, a2=0.3, b=1");
    EXPECT_NE(m.equation().find("a1*y(t)"), std::string::npos);
    EXPECT_DOUBLE_EQ(m.timeStep(), 1.0);
}

TEST(LinearModel13, IsStableUsesTheStabilityAnalyzer) {
    std::string info;
    EXPECT_TRUE(LinearModel13(0.5, 0.3, 1.0, 0.0).isStable(info));
    EXPECT_NE(info.find("max|z|"), std::string::npos);

    EXPECT_FALSE(LinearModel13(1.2, 0.5, 1.0, 0.0).isStable(info));
    EXPECT_NE(info.find("max|z|"), std::string::npos);
}
