#include <gtest/gtest.h>
#include <cmath>
#include <memory>
#include <tuple>
#include <vector>
#include "../../task_01/scr/model11.h"
#include "../../task_01/scr/model28.h"
#include "../../task_01/scr/model36.h"

struct Model11Case { double a, b, u, expected; };
class Model11ParameterizedTest : public ::testing::TestWithParam<Model11Case> {};
TEST_P(Model11ParameterizedTest, CalculatesExpectedNextStep) {
    const auto p = GetParam();
    Model11 model(p.a, p.b);
    EXPECT_NEAR(model.nextStep(p.u, 0.1), p.expected, 1e-12);
}
INSTANTIATE_TEST_SUITE_P(VariousCoefficients, Model11ParameterizedTest,
    ::testing::Values(Model11Case{0.5, 2.0, 1.0, 2.0},
                      Model11Case{0.2, 1.5, 2.0, 3.0},
                      Model11Case{-0.5, 1.0, 2.0, 2.0},
                      Model11Case{0.0, 3.0, -2.0, -6.0}));

struct Model28Case { double a, b, c, u, expected; };
class Model28ParameterizedTest : public ::testing::TestWithParam<Model28Case> {};
TEST_P(Model28ParameterizedTest, CalculatesLogisticMapAndDisturbance) {
    const auto p = GetParam();
    Model28 model(p.a, p.b, p.c);
    EXPECT_NEAR(model.nextStep(p.u, 0.1), p.expected, 1e-12);
}
INSTANTIATE_TEST_SUITE_P(VariousCoefficients, Model28ParameterizedTest,
    ::testing::Values(Model28Case{2.0, 1.0, 0.0, 0.5, 0.5},
                      Model28Case{3.0, 2.0, 0.0, 1.0, 2.0},
                      Model28Case{0.0, 2.0, 0.0, -1.0, -2.0}));

struct Model36Case { double a, b, u, dt, expected; };
class Model36ParameterizedTest : public ::testing::TestWithParam<Model36Case> {};
TEST_P(Model36ParameterizedTest, PerformsEulerIntegrationStep) {
    const auto p = GetParam();
    Model36 model(p.a, p.b);
    EXPECT_NEAR(model.nextStep(p.u, p.dt), p.expected, 1e-12);
}
INSTANTIATE_TEST_SUITE_P(VariousInputs, Model36ParameterizedTest,
    ::testing::Values(Model36Case{1.0, 2.0, 3.0, 0.1, 0.6},
                      Model36Case{0.0, 4.0, 2.0, 0.25, 2.0},
                      Model36Case{-1.0, 1.0, 0.0, 0.1, 0.0}));

TEST(ModelInvariantTest, ZeroInputAndZeroInitialStateRemainAtRest) {
    Model11 linear(0.5, 2.0);
    Model28 nonlinear(3.5, 1.0, 0.2);
    Model36 differential(0.5, 1.0);
    EXPECT_DOUBLE_EQ(linear.nextStep(0.0, 0.1), 0.0);
    EXPECT_DOUBLE_EQ(nonlinear.nextStep(0.0, 0.1), 0.0);
    EXPECT_DOUBLE_EQ(differential.nextStep(0.0, 0.1), 0.0);
}

TEST(ModelResetTest, ResetRestoresInitialState) {
    Model11 model(0.5, 2.0);
    EXPECT_DOUBLE_EQ(model.nextStep(1.0, 0.1), 2.0);
    model.reset();
    EXPECT_DOUBLE_EQ(model.nextStep(0.0, 0.1), 0.0);
}

TEST(InputSignalTest, StepPulseAndHarmonicSignalsAreInjectedAsExpected) {
    // The public model API accepts u directly, so signal generation remains a
    // deterministic test-side fixture rather than duplicating production code.
    Model11 step(0.0, 1.0), pulse(0.0, 1.0), harmonic(0.0, 1.0);
    EXPECT_DOUBLE_EQ(step.nextStep(2.0, 0.1), 2.0);
    EXPECT_DOUBLE_EQ(pulse.nextStep(3.0, 0.1), 3.0);
    EXPECT_DOUBLE_EQ(pulse.nextStep(0.0, 0.1), 0.0);
    EXPECT_NEAR(harmonic.nextStep(2.0 * std::sin(0.1), 0.1), 2.0 * std::sin(0.1), 1e-12);
}
