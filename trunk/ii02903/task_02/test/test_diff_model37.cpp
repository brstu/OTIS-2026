// Tests of Model 3.7:  dy/dt = -exp(a*y) + b*u,  Euler method  y(k+1) = y(k) + dt*(-exp(a*y(k)) + b*u(k))
#include <cmath>
#include <tuple>

#include <gtest/gtest.h>

#include "models.h"

namespace {

// Exact solution for u = 0, y(0) = 0:  dy/dt = -exp(a*y)  =>  y(t) = -ln(1 + a*t) / a
double exactFreeMotion(double a, double t) { return -std::log(1.0 + a * t) / a; }

// Runs Euler with the step dt up to the time T and returns |y_euler(T) - y_exact(T)| for u = 0
double eulerError(double dt, double T) {
    const double a = 0.5;
    DiffModel37 m(a, 2.0, dt, 0.0);
    const int steps = static_cast<int>(std::lround(T / dt));
    for (int i = 0; i < steps; ++i) m.nextStep(0.0);
    return std::fabs(m.current() - exactFreeMotion(a, T));
}

}  // namespace

// ---------- 1. values calculated by hand ----------
// a = 0.5, b = 2, dt = 0.1, y(0) = 0, step u = 1:
//   y1 = 0 + 0.1*(-e^0 + 2)             = 0.1
//   y2 = 0.1 + 0.1*(-e^0.05 + 2)        = 0.19487289036
//   y3 = 0.19487289 + 0.1*(-e^0.0974364 + 2) = 0.28463875234
TEST(DiffModel37, EulerStepsMatchHandCalculation) {
    DiffModel37 m(0.5, 2.0, 0.1, 0.0);
    EXPECT_NEAR(m.nextStep(1.0), 0.1, 1e-12);
    EXPECT_NEAR(m.nextStep(1.0), 0.1948728903623976, 1e-12);
    EXPECT_NEAR(m.nextStep(1.0), 0.2846387523393672, 1e-12);
}

TEST(DiffModel37, TimeStepIsTheStepOfIntegration) {
    EXPECT_DOUBLE_EQ(DiffModel37(0.5, 2.0, 0.1, 0.0).timeStep(), 0.1);
    EXPECT_DOUBLE_EQ(DiffModel37(0.5, 2.0, 0.25, 0.0).timeStep(), 0.25);
}

TEST(DiffModel37, CurrentIsInitialValueBeforeFirstStep) {
    EXPECT_DOUBLE_EQ(DiffModel37(0.5, 2.0, 0.1, 1.5).current(), 1.5);
}

// ---------- 4. invariants ----------
// Zero input and zero state are NOT a rest state here: dy/dt = -exp(0) = -1 != 0,
// so the first Euler step goes to y = -dt.
TEST(DiffModel37, ZeroStateWithZeroInputIsNotAtRest) {
    DiffModel37 m(0.5, 2.0, 0.1, 0.0);
    EXPECT_NEAR(m.nextStep(0.0), -0.1, 1e-12);
}

// The real equilibrium is the point where -exp(a*y) + b*u = 0, i.e. y* = ln(b*u)/a.
// If the model starts there, the output must not move.
TEST(DiffModel37, EquilibriumPointDoesNotMove) {
    const double a = 0.5;
    const double b = 2.0;
    const double u = 1.0;
    const double yStar = std::log(b * u) / a;

    DiffModel37 m(a, b, 0.1, yStar);
    for (int i = 0; i < 100; ++i) EXPECT_NEAR(m.nextStep(u), yStar, 1e-12);
}

// ---------- comparison with the analytical solution (Euler step correctness) ----------
TEST(DiffModel37, EulerIsCloseToAnalyticalSolution) {
    // u = 0, y(0) = 0, a = 0.5: y(1) = -2*ln(1.5) = -0.81093021622
    EXPECT_NEAR(exactFreeMotion(0.5, 1.0), -0.8109302162163288, 1e-12);
    EXPECT_NEAR(eulerError(0.001, 1.0), 0.0, 2e-4);
}

class EulerErrorTest : public ::testing::TestWithParam<double> {};

// Euler is a first-order method: the error at t = 1 is about 0.137*dt.
TEST_P(EulerErrorTest, ErrorIsProportionalToStep) {
    const double dt = GetParam();
    const double err = eulerError(dt, 1.0);
    EXPECT_GT(err, 0.0);
    EXPECT_LT(err, 0.2 * dt);
}

// Halving the step must halve the error.
TEST_P(EulerErrorTest, HalvingTheStepHalvesTheError) {
    const double dt = GetParam();
    const double ratio = eulerError(dt, 1.0) / eulerError(dt / 2.0, 1.0);
    EXPECT_NEAR(ratio, 2.0, 0.15);
}

INSTANTIATE_TEST_SUITE_P(Steps, EulerErrorTest, ::testing::Values(0.1, 0.05, 0.025, 0.0125));

// ---------- parameterized: convergence to the equilibrium y* = ln(b*u)/a ----------
class DiffConvergenceTest : public ::testing::TestWithParam<std::tuple<double, double, double>> {};

TEST_P(DiffConvergenceTest, ConvergesToEquilibrium) {
    const double a = std::get<0>(GetParam());
    const double b = std::get<1>(GetParam());
    const double u = std::get<2>(GetParam());

    DiffModel37 m(a, b, 0.05, 0.0);
    for (int i = 0; i < 4000; ++i) m.nextStep(u);

    EXPECT_NEAR(m.current(), std::log(b * u) / a, 1e-6);
}

INSTANTIATE_TEST_SUITE_P(Sets, DiffConvergenceTest,
                         ::testing::Values(std::make_tuple(0.5, 2.0, 1.0),
                                           std::make_tuple(1.0, 3.0, 1.0),
                                           std::make_tuple(0.2, 1.0, 2.0)));

// ---------- information about the model ----------
TEST(DiffModel37, DescribesItself) {
    const DiffModel37 m(0.5, 2.0, 0.1, 0.0);
    EXPECT_EQ(m.tag(), "model37");
    EXPECT_NE(m.name().find("3.7"), std::string::npos);
    EXPECT_EQ(m.params(), "a=0.5, b=2, dt=0.1");
    EXPECT_NE(m.equation().find("dy/dt"), std::string::npos);
}

TEST(DiffModel37, HasNoStabilityAnalysis) {
    std::string info = "something";
    EXPECT_TRUE(DiffModel37(0.5, 2.0, 0.1, 0.0).isStable(info));
    EXPECT_TRUE(info.empty());
}
