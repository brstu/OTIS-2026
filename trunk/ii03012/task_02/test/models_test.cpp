#include "DiffEquations.h"
#include "LinearModels.h"
#include "NonLinearModels.h"
#include "Simulator.h"
#include <gtest/gtest.h>
#include <array>
#include <cmath>
#include <limits>
#include <stdexcept>

struct PoleCase { double a; PoleLocation expected; };
class PoleTest : public testing::TestWithParam<PoleCase> {};
TEST_P(PoleTest, ClassifiesRootIndependentlyOfInputGain)
{
    const auto [a, expected] = GetParam();
    for (double b : {-2., 0., 3.}) {
        Model1_4 model(a, b);
        EXPECT_EQ(model.poleLocation(), expected);
        EXPECT_EQ(model.isStable(), expected == PoleLocation::Inside);
    }
}
INSTANTIATE_TEST_SUITE_P(UnitCircle, PoleTest, testing::Values(
    PoleCase{0, PoleLocation::Inside}, PoleCase{std::nextafter(1., 0.), PoleLocation::Inside},
    PoleCase{std::nextafter(-1., 0.), PoleLocation::Inside}, PoleCase{1, PoleLocation::Boundary},
    PoleCase{-1, PoleLocation::Boundary}, PoleCase{std::nextafter(1., 2.), PoleLocation::Outside},
    PoleCase{std::nextafter(-1., -2.), PoleLocation::Outside}));
TEST(Stability, RejectsInvalidCoefficients)
{
    EXPECT_THROW(locatePole(std::numeric_limits<double>::infinity()), std::invalid_argument);
    EXPECT_THROW(locatePole(std::numeric_limits<double>::quiet_NaN()), std::invalid_argument);
}

struct StepCase {
    double y0, firstInput, secondInput;
    std::array<double, 2> linear, nonlinear, euler;
};
class StepTest : public testing::TestWithParam<StepCase> {};
TEST_P(StepTest, MatchesTwoHandCalculatedStepsAndReset)
{
    const auto& c = GetParam();
    Model1_4 linear(0.5, 2);
    Model2_6 nonlinear(1, 0.5, 2);
    Model3_8 euler(1, 0.5, 2, 0.5);
    auto check = [&](Model& model, const std::array<double, 2>& expected) {
        for (int run = 0; run < 2; ++run) {
            model.reset(c.y0);
            EXPECT_DOUBLE_EQ(model.step(c.firstInput), expected[0]);
            EXPECT_DOUBLE_EQ(model.step(c.secondInput), expected[1]);
            EXPECT_DOUBLE_EQ(model.currentY(), expected[1]);
        }
    };
    check(linear, c.linear);
    check(nonlinear, c.nonlinear);
    check(euler, c.euler);
}
INSTANTIATE_TEST_SUITE_P(Trajectories, StepTest, testing::Values(
    StepCase{0, 0, 0, {0, 0}, {0, 0}, {0, 0}},
    StepCase{1, 1, 0, {2.5, -0.75}, {2.5, 15.125}, {1.25, 0.234375}},
    StepCase{-1, -1, 1, {-2.5, 2.75}, {1.5, 5.875}, {-1.75, -0.640625}}));

class DecayTest : public testing::TestWithParam<double> {};
TEST_P(DecayTest, FreeResponseMatchesClosedForm)
{
    const double a = GetParam();
    Model1_4 model(a, 3);
    const auto rows = Simulator::run(model, StepSignal(0), 30, -2);
    ASSERT_EQ(rows.size(), 31u);
    for (const auto& row : rows)
        EXPECT_NEAR(row.y, -2 * std::pow(a, row.tau), 1e-11);
}
INSTANTIATE_TEST_SUITE_P(Poles, DecayTest, testing::Values(-1.2, -1., -.8, 0., .8, 1., 1.2));

TEST(Euler, HalvingStepImprovesAccuracyAtSameFinalTime)
{
    auto error = [](int steps) {
        Model3_8 model(1, 1, 0, 1. / steps);
        const auto rows = Simulator::run(model, StepSignal(0), steps, 1);
        // y' = -y - y², y(0) = 1 => y(1) = 1 / (2e - 1).
        return std::abs(rows.back().y - 1. / (2 * std::exp(1.) - 1));
    };
    EXPECT_LT(error(40), 0.55 * error(20));
}
TEST(Euler, RejectsInvalidTimeStep)
{
    for (double dt : {0., -1., std::numeric_limits<double>::infinity()})
        EXPECT_THROW(Model3_8(1, 1, 1, dt), std::invalid_argument);
}

class RecordedSignal final : public InputSignal {
public:
    double value(int tau) const override {
        samples.push_back(tau);
        return values.at(tau);
    }
    std::string name() const override { return "recorded"; }
    std::string id() const override { return "recorded"; }
    mutable std::vector<int> samples;
    std::array<double, 4> values;
};
class SignalReplayTest : public testing::TestWithParam<std::array<double, 4>> {};
TEST_P(SignalReplayTest, SimulatorUsesEachSampleOnceAtCorrectTime)
{
    RecordedSignal input;
    input.values = GetParam();
    Model1_4 model(1, 2);
    const auto rows = Simulator::run(model, input, 3, 1);
    ASSERT_EQ(rows.size(), 4u);
    EXPECT_EQ(input.samples, (std::vector<int>{0, 1, 2, 3}));
    for (int t = 0; t <= 3; ++t) {
        EXPECT_EQ(rows[t].tau, t);
        EXPECT_DOUBLE_EQ(rows[t].u, input.values[t]);
        EXPECT_DOUBLE_EQ(rows[t].y, t == 0 ? 1. : 1. + 2 * input.values[t-1]);
    }
}
using Samples = std::array<double, 4>;
INSTANTIATE_TEST_SUITE_P(Inputs, SignalReplayTest, testing::Values(
    Samples{2,2,2,2}, Samples{2,0,0,0},
    Samples{0, 0.8414709848078965, 0.9092974268256817, 0.1411200080598672}));

TEST(Signals, RealInputsMatchDefinitions)
{
    for (int t = 0; t < 8; ++t) {
        EXPECT_DOUBLE_EQ(StepSignal(-2).value(t), -2.);
        EXPECT_DOUBLE_EQ(ImpulseSignal(-2).value(t), t == 0 ? -2. : 0.);
        EXPECT_DOUBLE_EQ(HarmonicSignal(-2).value(t), -2 * std::sin(t));
    }
}
TEST(Simulator, ZeroAndNegativeStepCounts)
{
    Model1_4 model(0, 1);
    const auto rows = Simulator::run(model, StepSignal(), 0, 4);
    ASSERT_EQ(rows.size(), 1u);
    EXPECT_DOUBLE_EQ(rows.front().y, 4.);
    EXPECT_THROW(Simulator::run(model, StepSignal(), -1, 0), std::invalid_argument);
}
TEST(Simulator, KeepsLastValidRowOnDivergence)
{
    Model2_6 model(1, 0, 0);
    const auto rows = Simulator::run(model, StepSignal(0), 10, 100);
    ASSERT_EQ(rows.size(), 2u);
    EXPECT_EQ(rows.back().y, Simulator::kDivergenceLimit);
    Model2_6 overflow(1e308, 0, 0);
    EXPECT_EQ(Simulator::run(overflow, StepSignal(0), 10, 100).size(), 1u);
}
TEST(Simulator, ReportsCsvOpenError)
{
    EXPECT_FALSE(Simulator::saveCsv({}, ""));
}
