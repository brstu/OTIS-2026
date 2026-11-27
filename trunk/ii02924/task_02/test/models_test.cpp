#include "models.hpp"
#include "stability.hpp"
#include <gtest/gtest.h>
#include <cmath>
#include <tuple>

TEST(LinearModel19, KnownFirstThreeValues) {
    LinearModel19 model;
    EXPECT_NEAR(model.next(1.0), 0.4, 1e-12);
    EXPECT_NEAR(model.next(1.0), 0.6, 1e-12);
    EXPECT_NEAR(model.next(1.0), 0.78, 1e-12);
}

TEST(NonlinearModel21, KnownFirstThreeValues) {
    NonlinearModel21 model;
    EXPECT_NEAR(model.next(1.0, 0.0), 0.4, 1e-12);
    EXPECT_NEAR(model.next(1.0, 1.0), 0.68414709848, 1e-9);
    EXPECT_NEAR(model.next(1.0, 1.0), 0.8182206477, 1e-9);
}

TEST(DifferentialModel33, EulerStep) {
    DifferentialModel33 model;
    EXPECT_NEAR(model.next(1.0), 0.1, 1e-12);
    EXPECT_NEAR(model.next(1.0), 0.194, 1e-12);
}

TEST(Models, ResetRestoresInitialState) {
    LinearModel19 model;
    model.next(1.0); model.next(1.0); model.reset();
    EXPECT_NEAR(model.next(1.0), 0.4, 1e-12);
}

TEST(Models, AllResetMethodsAndParameterGetterWork) {
    NonlinearModel21 nonlinear; nonlinear.next(1.0); nonlinear.reset();
    EXPECT_NEAR(nonlinear.next(1.0), 0.4, 1e-12);
    DifferentialModel33 differential; differential.next(1.0); differential.reset();
    EXPECT_NEAR(differential.next(1.0), 0.1, 1e-12);
    LinearModel19 linear;
    const auto p = linear.parameters();
    EXPECT_DOUBLE_EQ(p.a1, 0.5);
    EXPECT_DOUBLE_EQ(p.b1, 0.4);
}

TEST(Signals, NamesAreReadable) {
    EXPECT_EQ(signalName(SignalType::Step), "step");
    EXPECT_EQ(signalName(SignalType::Impulse), "impulse");
    EXPECT_EQ(signalName(SignalType::Harmonic), "harmonic");
}

TEST(Signals, AmplitudeScalesSignal) {
    EXPECT_DOUBLE_EQ(signalValue(SignalType::Step, 5, 2.5), 2.5);
    EXPECT_DOUBLE_EQ(signalValue(SignalType::Impulse, 1, 2.5), 0.0);
    EXPECT_NEAR(signalValue(SignalType::Harmonic, 1, 2.0),
                2.0 * std::sin(1.0), 1e-12);
}

TEST(Models, ZeroInputAndZeroStateRemainAtRest) {
    LinearModel19 linear; NonlinearModel21 nonlinear; DifferentialModel33 differential;
    for (int i = 0; i < 10; ++i) {
        EXPECT_DOUBLE_EQ(linear.next(0.0), 0.0);
        EXPECT_DOUBLE_EQ(nonlinear.next(0.0, 0.0), 0.0);
        EXPECT_DOUBLE_EQ(differential.next(0.0), 0.0);
    }
}

class SignalTest : public testing::TestWithParam<std::tuple<SignalType, int, double>> {};
TEST_P(SignalTest, ReturnsExpectedValue) {
    const auto [type, step, expected] = GetParam();
    EXPECT_NEAR(signalValue(type, step), expected, 1e-9);
}
INSTANTIATE_TEST_SUITE_P(AllControlModes, SignalTest, testing::Values(
    std::make_tuple(SignalType::Step, 0, 1.0),
    std::make_tuple(SignalType::Step, 5, 1.0),
    std::make_tuple(SignalType::Impulse, 0, 1.0),
    std::make_tuple(SignalType::Impulse, 5, 0.0),
    std::make_tuple(SignalType::Harmonic, 0, 0.0),
    std::make_tuple(SignalType::Harmonic, 1, 0.8414709848)));

struct LinearCase { LinearParameters p; double u; double expected; };
class LinearStepTest : public testing::TestWithParam<LinearCase> {};
TEST_P(LinearStepTest, FirstStepMatchesManualCalculation) {
    const auto test = GetParam();
    LinearModel19 model(test.p);
    EXPECT_NEAR(model.next(test.u), test.expected, 1e-12);
}
INSTANTIATE_TEST_SUITE_P(SeveralCoefficients, LinearStepTest, testing::Values(
    LinearCase{{0.5, 0.2, 0.1, 0.4}, 1.0, 0.4},
    LinearCase{{0.2, 0.1, 0.05, 0.5}, 2.0, 1.0},
    LinearCase{{0.0, 0.0, 0.0, 1.0}, -2.0, -2.0}));

TEST(Stability, DefaultVariantIsStable) {
    EXPECT_TRUE(analyzeStability({0.5, 0.2, 0.1, 0.4}).stable);
}
TEST(Stability, RootOutsideUnitCircleIsUnstable) {
    EXPECT_FALSE(analyzeStability({1.2, 0.0, 0.0, 1.0}).stable);
}
TEST(Stability, RootOnUnitCircleIsNotStable) {
    EXPECT_FALSE(analyzeStability({1.0, 0.0, 0.0, 1.0}).stable);
}
TEST(Stability, SimpleTripleZeroIsStable) {
    const auto result = analyzeStability({0.0, 0.0, 0.0, 1.0});
    EXPECT_TRUE(result.stable);
    for (const auto& root : result.roots) EXPECT_LT(std::abs(root), 1e-5);
}
