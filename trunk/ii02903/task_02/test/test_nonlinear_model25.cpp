// Tests of Model 2.5:  y(tau+1) = a*y(tau) + b*sign(u(tau))*(1 - exp(-|u(tau)|))
#include <cmath>
#include <tuple>
#include <vector>

#include <gtest/gtest.h>

#include "models.h"

// ---------- 1. values calculated by hand ----------
// a = 0.5, b = 2, y(0) = 0, step u = 1.  The added term is 2*(1 - e^-1) = 1.26424111765...
//   y1 = 1.26424111765
//   y2 = 0.5*1.26424111765 + 1.26424111765 = 1.89636167649
//   y3 = 0.5*1.89636167649 + 1.26424111765 = 2.21242195590
TEST(NonlinearModel25, StepResponseMatchesHandCalculation) {
    NonlinearModel25 m(0.5, 2.0, 0.0);
    EXPECT_NEAR(m.nextStep(1.0), 1.2642411176571153, 1e-9);
    EXPECT_NEAR(m.nextStep(1.0), 1.896361676485673, 1e-9);
    EXPECT_NEAR(m.nextStep(1.0), 2.212421955899952, 1e-9);
}

// Input u = 2, -1, 0, 0.5 (a = 0.5, b = 2):
//   y1 =  2*(1 - e^-2)               = 1.72932943352677
//   y2 =  0.5*y1 - 2*(1 - e^-1)      = -0.39957640089373
//   y3 =  0.5*y2 + 0 (sign(0) = 0)   = -0.19978820044686
//   y4 =  0.5*y3 + 2*(1 - e^-0.5)    = 0.68704458035130
TEST(NonlinearModel25, MixedInputSignsMatchHandCalculation) {
    NonlinearModel25 m(0.5, 2.0, 0.0);
    EXPECT_NEAR(m.nextStep(2.0), 1.7293294335267746, 1e-9);
    EXPECT_NEAR(m.nextStep(-1.0), -0.39957640089372803, 1e-9);
    EXPECT_NEAR(m.nextStep(0.0), -0.19978820044686402, 1e-9);
    EXPECT_NEAR(m.nextStep(0.5), 0.6870445803513011, 1e-9);
}

// ---------- 3. boundary values of the non-linearity ----------
TEST(NonlinearModel25, ZeroInputOnlyDampsTheOutput) {
    // sign(0) = 0: the input term disappears, y(tau+1) = a*y(tau)
    NonlinearModel25 m(0.8, 1.0, 5.0);
    EXPECT_DOUBLE_EQ(m.nextStep(0.0), 4.0);
    EXPECT_DOUBLE_EQ(m.nextStep(0.0), 3.2);
}

TEST(NonlinearModel25, VerySmallInputGivesVerySmallAddition) {
    // 1 - exp(-|u|) is about |u| for small u, so the output is continuous at u = 0
    NonlinearModel25 pos(0.5, 2.0, 0.0);
    NonlinearModel25 neg(0.5, 2.0, 0.0);
    EXPECT_NEAR(pos.nextStep(1e-12), 2e-12, 1e-15);
    EXPECT_NEAR(neg.nextStep(-1e-12), -2e-12, 1e-15);
}

TEST(NonlinearModel25, VeryLargeInputSaturatesAtB) {
    // 1 - exp(-|u|) < 1, so one step can never add more than |b|
    NonlinearModel25 pos(0.5, 2.0, 0.0);
    NonlinearModel25 neg(0.5, 2.0, 0.0);
    EXPECT_NEAR(pos.nextStep(50.0), 2.0, 1e-9);
    EXPECT_NEAR(neg.nextStep(-50.0), -2.0, 1e-9);
    EXPECT_LT(pos.current(), 2.0 + 1e-12);
}

// The model is NOT linear: the input is 10 times bigger, the output is only 1.6 times bigger.
TEST(NonlinearModel25, ResponseIsNotProportionalToInput) {
    NonlinearModel25 m1(0.5, 2.0, 0.0);
    NonlinearModel25 m10(0.5, 2.0, 0.0);
    const double y1 = m1.nextStep(1.0);
    const double y10 = m10.nextStep(10.0);
    EXPECT_LT(y10 / y1, 2.0);
    EXPECT_GT(y10, y1);
}

// ---------- parameterized: the model is odd, f(-u) = -f(u) ----------
class NonlinearSymmetryTest : public ::testing::TestWithParam<double> {};

TEST_P(NonlinearSymmetryTest, NegativeInputGivesOppositeOutput) {
    const double u = GetParam();
    NonlinearModel25 plus(0.7, 1.5, 0.0);
    NonlinearModel25 minus(0.7, 1.5, 0.0);
    EXPECT_NEAR(minus.nextStep(-u), -plus.nextStep(u), 1e-12);
}

INSTANTIATE_TEST_SUITE_P(Inputs, NonlinearSymmetryTest, ::testing::Values(0.01, 0.5, 1.0, 3.0, 20.0));

// ---------- 4. invariants ----------
TEST(NonlinearModel25, ZeroInputAndZeroStateStayAtRest) {
    NonlinearModel25 m(0.8, 1.0, 0.0);
    for (int i = 0; i < 50; ++i) EXPECT_DOUBLE_EQ(m.nextStep(0.0), 0.0);
}

// ---------- parameterized: steady state under a step ----------
// y = b*sign(u)*(1 - exp(-|u|)) / (1 - a)  for |a| < 1
class NonlinearConvergenceTest : public ::testing::TestWithParam<std::tuple<double, double>> {};

TEST_P(NonlinearConvergenceTest, ConvergesToSteadyState) {
    const double a = std::get<0>(GetParam());
    const double u = std::get<1>(GetParam());
    const double b = 1.5;

    NonlinearModel25 m(a, b, 0.0);
    for (int i = 0; i < 500; ++i) m.nextStep(u);

    const double sign = u > 0.0 ? 1.0 : -1.0;
    const double expected = b * sign * (1.0 - std::exp(-std::fabs(u))) / (1.0 - a);
    EXPECT_NEAR(m.current(), expected, 1e-9);
}

INSTANTIATE_TEST_SUITE_P(Grid, NonlinearConvergenceTest,
                         ::testing::Combine(::testing::Values(0.8, 0.5, -0.3),
                                            ::testing::Values(1.0, -2.0, 0.1, 5.0)));

// ---------- information about the model ----------
TEST(NonlinearModel25, DescribesItself) {
    const NonlinearModel25 m(0.8, 1.0, 0.0);
    EXPECT_EQ(m.tag(), "model25");
    EXPECT_NE(m.name().find("2.5"), std::string::npos);
    EXPECT_EQ(m.params(), "a=0.8, b=1");
    EXPECT_NE(m.equation().find("exp"), std::string::npos);
    EXPECT_DOUBLE_EQ(m.timeStep(), 1.0);
}

TEST(NonlinearModel25, HasNoStabilityAnalysis) {
    // the stability check is defined only for the linear model: the base class answers "nothing to check"
    std::string info = "something";
    EXPECT_TRUE(NonlinearModel25(0.8, 1.0, 0.0).isStable(info));
    EXPECT_TRUE(info.empty());
}
