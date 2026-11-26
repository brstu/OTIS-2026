#include "input_signal.h"
#include "models.h"
#include "stability_analyzer.h"

#include <cmath>
#include <limits>
#include <stdexcept>
#include <string>
#include <vector>

#include <gtest/gtest.h>

struct Model14Scenario {
    double a;
    double b;
    double initialOutput;
    std::vector<double> inputs;
    std::vector<double> expectedOutputs;
};

class Model14StepTest : public testing::TestWithParam<Model14Scenario> {};

std::string model14ScenarioName(const testing::TestParamInfo<Model14Scenario>& info)
{
    static const char* const names[] = {"Step", "Impulse", "Harmonic"};
    return names[info.index];
}

TEST_P(Model14StepTest, CalculatesEachStepFromCurrentAndPreviousInputs)
{
    const auto& scenario = GetParam();
    Model14 model(scenario.a, scenario.b, scenario.initialOutput);
    for (std::size_t i = 0; i < scenario.inputs.size(); ++i) {
        EXPECT_NEAR(model.nextStep(scenario.inputs[i]), scenario.expectedOutputs[i], 1e-12);
    }
}

INSTANTIATE_TEST_SUITE_P(
    StepImpulseAndHarmonicInputs,
    Model14StepTest,
    testing::Values(
        Model14Scenario{0.5, 2.0, 0.0, {1.0, 1.0, 1.0}, {2.0, 1.0, 0.5}},
        Model14Scenario{0.5, 2.0, 0.0, {1.0, 0.0, 0.0}, {2.0, -1.0, -0.5}},
        Model14Scenario{0.5, 2.0, 0.0, {0.0, std::sin(1.0), std::sin(2.0)},
            {0.0, 2.0 * std::sin(1.0), 2.0 * std::sin(2.0) - std::sin(1.0)}}),
    model14ScenarioName);

struct SignalScenario {
    InputSignal signal;
    double stepValue;
    std::vector<double> expected;
};

class InputSignalTest : public testing::TestWithParam<SignalScenario> {};

std::string inputSignalScenarioName(const testing::TestParamInfo<SignalScenario>& info)
{
    static const char* const names[] = {"Step", "Impulse", "Harmonic"};
    return names[info.index];
}

TEST_P(InputSignalTest, GeneratesTheRequestedControlSequence)
{
    const auto& scenario = GetParam();
    for (std::size_t tau = 0; tau < scenario.expected.size(); ++tau) {
        EXPECT_NEAR(inputAt(scenario.signal, static_cast<int>(tau), scenario.stepValue),
                    scenario.expected[tau], 1e-12);
    }
}

INSTANTIATE_TEST_SUITE_P(
    SupportedSignalModes,
    InputSignalTest,
    testing::Values(
        SignalScenario{InputSignal::Step, 2.5, {2.5, 2.5, 2.5}},
        SignalScenario{InputSignal::Impulse, 0.0, {1.0, 0.0, 0.0}},
        SignalScenario{InputSignal::Harmonic, 0.0, {0.0, std::sin(1.0), std::sin(2.0)}}),
    inputSignalScenarioName);

TEST(Model26Test, CalculatesCubicFeedbackAndPreviousOutput)
{
    Model26 model(0.2, 0.3, 0.5, 0.0);
    EXPECT_DOUBLE_EQ(model.nextStep(0.0), 0.0);
    EXPECT_DOUBLE_EQ(model.nextStep(1.0), 0.5);
    EXPECT_DOUBLE_EQ(model.nextStep(0.0), 0.025);
}

TEST(Model38Test, AdvancesOneEulerStepUsingCurrentState)
{
    Model38 model(0.5, 0.1, 1.0, 0.1, 0.0);
    EXPECT_NEAR(model.nextStep(1.0), 0.1, 1e-12);
    EXPECT_NEAR(model.nextStep(1.0), 0.1949, 1e-12);
}

TEST(ModelTest, AllModelsRemainAtRestWithZeroInputAndInitialState)
{
    Model14 model14(0.7, 0.5, 0.0);
    Model26 model26(0.2, 0.3, 0.5, 0.0);
    Model38 model38(0.5, 0.1, 1.0, 0.1, 0.0);

    EXPECT_DOUBLE_EQ(model14.nextStep(0.0), 0.0);
    EXPECT_DOUBLE_EQ(model26.nextStep(0.0), 0.0);
    EXPECT_DOUBLE_EQ(model38.nextStep(0.0), 0.0);
}

TEST(Model38Test, RejectsNonPositiveTimeStep)
{
    EXPECT_THROW(Model38(1.0, 1.0, 1.0, 0.0, 0.0), std::invalid_argument);
    EXPECT_THROW(Model38(1.0, 1.0, 1.0, -0.1, 0.0), std::invalid_argument);
    EXPECT_THROW(Model38(1.0, 1.0, 1.0, std::numeric_limits<double>::infinity(), 0.0),
                 std::invalid_argument);
}

TEST(StabilityAnalyzerTest, AcceptsRootsStrictlyInsideUnitCircle)
{
    EXPECT_TRUE(StabilityAnalyzer::isModel14Stable(0.0));
    EXPECT_TRUE(StabilityAnalyzer::isModel14Stable(0.9));
    EXPECT_TRUE(StabilityAnalyzer::isModel14Stable(-0.9));
}

TEST(StabilityAnalyzerTest, RejectsRootsOutsideUnitCircle)
{
    EXPECT_FALSE(StabilityAnalyzer::isModel14Stable(1.1));
    EXPECT_FALSE(StabilityAnalyzer::isModel14Stable(-1.1));
}

TEST(StabilityAnalyzerTest, TreatsUnitCircleBoundaryAsNotAsymptoticallyStable)
{
    EXPECT_FALSE(StabilityAnalyzer::isModel14Stable(1.0));
    EXPECT_FALSE(StabilityAnalyzer::isModel14Stable(-1.0));
}

TEST(StabilityAnalyzerTest, RejectsNonFiniteCoefficient)
{
    EXPECT_THROW(StabilityAnalyzer::isModel14Stable(std::numeric_limits<double>::quiet_NaN()),
                 std::invalid_argument);
    EXPECT_THROW(StabilityAnalyzer::isModel14Stable(std::numeric_limits<double>::infinity()),
                 std::invalid_argument);
}

TEST(ModelTest, NamesIdentifyEachVariantModel)
{
    EXPECT_EQ(Model14(0.5, 1.0, 0.0).name(), "Модель 1.4");
    EXPECT_EQ(Model26(0.5, 1.0, 1.0, 0.0).name(), "Модель 2.6");
    EXPECT_EQ(Model38(0.5, 1.0, 1.0, 0.1, 0.0).name(), "Модель 3.8");
}

TEST(InputSignalTest, RejectsUnknownSignalType)
{
    EXPECT_THROW(inputAt(static_cast<InputSignal>(99), 0), std::invalid_argument);
}
