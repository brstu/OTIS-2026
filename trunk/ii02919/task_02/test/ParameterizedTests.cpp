#include <gtest/gtest.h>
#include "MultiStepModel.h"
#include "HarmonicODE.h"
#include <cmath>
#include <tuple>
#include <vector>

class MultiStepParameterizedTest : public ::testing::TestWithParam<std::tuple<double, double, double>> {};

TEST_P(MultiStepParameterizedTest, ComputesFirstStepForDifferentCoefficientsAndInputs)
{
    const auto [a, b1, u] = GetParam();
    MultiStepModel model(a, b1, 0.0, 0.0);
    // Initial y and input history are zero, so y[1] = b1*u.
    EXPECT_NEAR(model.advance(u), b1 * u, 1e-12);
}

INSTANTIATE_TEST_SUITE_P(VariousInputs, MultiStepParameterizedTest,
    ::testing::Values(
        std::make_tuple(0.0, 1.0, 2.0),
        std::make_tuple(0.4, 2.0, -1.5),
        std::make_tuple(-0.8, 0.5, 4.0),
        std::make_tuple(0.99, 3.0, 0.25),
        std::make_tuple(-0.99, -2.0, -3.0)));

class EulerParameterizedTest : public ::testing::TestWithParam<std::tuple<double, double, double>> {};

TEST_P(EulerParameterizedTest, OneEulerStepMatchesFormula)
{
    const auto [b, dt, u] = GetParam();
    HarmonicODE model(b, dt);
    EXPECT_NEAR(model.advance(u), dt * b * std::sin(u), 1e-12);
}

INSTANTIATE_TEST_SUITE_P(VariousEulerParameters, EulerParameterizedTest,
    ::testing::Values(
        std::make_tuple(1.0, 0.1, 0.5),
        std::make_tuple(2.0, 0.01, 1.0),
        std::make_tuple(-1.5, 0.2, -0.75),
        std::make_tuple(0.0, 0.05, 2.0)));
