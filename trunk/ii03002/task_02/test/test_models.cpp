#include <gtest/gtest.h>
#include <array>
#include <cmath>
#include "Model1_9.h"
#include "Model2_1.h"
#include "Model3_3.h"

struct LinearCase {
    double a1, a2, a3, b1;
    std::array<double, 4> u;
    std::array<double, 4> expected;
};

class Model19TrajectoryTest : public ::testing::TestWithParam<LinearCase> {};

TEST_P(Model19TrajectoryTest, ComputesExpectedSteps) {
    const auto& p = GetParam();
    Model1_9 model(p.a1, p.a2, p.a3, p.b1);
    for (std::size_t i = 0; i < p.u.size(); ++i)
        EXPECT_NEAR(model.step(p.u[i]), p.expected[i], 1e-10) << "step=" << i;
}

INSTANTIATE_TEST_SUITE_P(Inputs, Model19TrajectoryTest, ::testing::Values(
    LinearCase{0.4, 0.3, 0.1, 0.5, {1, 1, 1, 1}, {0.5, 0.7, 0.93, 1.132}},
    LinearCase{0.4, 0.3, 0.1, 0.5, {1, 0, 0, 0}, {0.5, 0.2, 0.23, 0.202}},
    LinearCase{0.4, 0.3, 0.1, 0.5, {0, 0, 0, 0}, {0, 0, 0, 0}}
));

TEST(Model19, ResetRestoresAllDelayedStates) {
    Model1_9 model(0.4, 0.3, 0.1, 0.5);
    model.step(1.0); model.step(0.0); model.reset();
    Model1_9 fresh(0.4, 0.3, 0.1, 0.5);
    EXPECT_DOUBLE_EQ(model.step(1.0), fresh.step(1.0));
}

TEST(Model21, KnownValuesAndDelayedInput) {
    Model2_1 model(0.5, 0.1, 0.4, 0.2);
    EXPECT_NEAR(model.step(1.0), 0.4, 1e-12);
    EXPECT_NEAR(model.step(0.0), 0.2 + 0.2 * std::sin(1.0), 1e-12);
}

TEST(Model21, ZeroInputAndZeroInitialStateRemainAtRest) {
    Model2_1 model(0.5, 0.1, 0.4, 0.2);
    for (int i = 0; i < 10; ++i) EXPECT_DOUBLE_EQ(model.step(0.0), 0.0);
}

TEST(Model21, ResetClearsPreviousInputAndState) {
    Model2_1 model(0.5, 0.1, 0.4, 0.2);
    model.step(1.0); model.reset();
    EXPECT_DOUBLE_EQ(model.step(0.0), 0.0);
}

TEST(Model33, EulerStepMatchesFormula) {
    Model3_3 model(0.5, 0.8, 0.1, 2.0);
    EXPECT_NEAR(model.step(1.0), 2.0 + (-0.5 * 2.0 + 0.8) * 0.1, 1e-12);
}

TEST(Model33, MultipleEulerStepsFollowRecurrence) {
    Model3_3 model(0.5, 0.8, 0.1);
    double expected = 0.0;
    for (double u : {1.0, 0.0, -1.0, 0.5}) {
        expected += (-0.5 * expected + 0.8 * u) * 0.1;
        EXPECT_NEAR(model.step(u), expected, 1e-12);
    }
}

TEST(Model33, ZeroInputAtZeroStateIsInvariant) {
    Model3_3 model(0.5, 0.8, 0.1);
    for (int i = 0; i < 10; ++i) EXPECT_DOUBLE_EQ(model.step(0.0), 0.0);
}

TEST(Model33, EulerConvergesToAnalyticalFirstOrderSolution) {
    constexpr double a = 0.5;
    constexpr double b = 0.8;
    constexpr double u = 1.0;
    constexpr double dt = 0.001;
    constexpr int steps = 1000;
    Model3_3 model(a, b, dt);
    double numerical = 0.0;
    for (int i = 0; i < steps; ++i) numerical = model.step(u);
    const double time = dt * steps;
    const double equilibrium = b * u / a;
    const double analytical = equilibrium * (1.0 - std::exp(-a * time));
    EXPECT_NEAR(numerical, analytical, 2e-4);
}
