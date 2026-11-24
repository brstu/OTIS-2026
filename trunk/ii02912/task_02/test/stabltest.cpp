
#include <gtest/gtest.h>
#include <limits>
#include <cmath>

#include "stabl.hpp"
#include "Model.hpp"
#include <gmock/gmock.h>
#include "Input.hpp"


class MockInput : public Input {
public:
    MOCK_METHOD(double, value, (int tau), (const, override));
    MOCK_METHOD(std::string, name, (), (const, override));
};


// Проверка устойчивых систем

TEST(StabilityTests, PositiveCoefficient) {
    auto result = StabilityAnalyzer::analyze(0.7);

    EXPECT_TRUE(result.stable);
    EXPECT_DOUBLE_EQ(result.root, 0.7);
}

TEST(StabilityTests, NegativeCoefficient) {
    auto result = StabilityAnalyzer::analyze(-0.8);

    EXPECT_TRUE(result.stable);
    EXPECT_DOUBLE_EQ(result.root, -0.8);
}

TEST(StabilityTests, CoefficientNearOne) {
    auto result = StabilityAnalyzer::analyze(0.99);

    EXPECT_TRUE(result.stable);
}

// Проверка границ устойчивости

TEST(StabilityTests, PositiveBoundary) {
    auto result = StabilityAnalyzer::analyze(1.0);

    EXPECT_FALSE(result.stable);
}

TEST(StabilityTests, NegativeBoundary) {
    auto result = StabilityAnalyzer::analyze(-1.0);

    EXPECT_FALSE(result.stable);
}

// Проверка неустойчивых систем

TEST(StabilityTests, CoefficientGreaterThanOne) {
    auto result = StabilityAnalyzer::analyze(1.5);

    EXPECT_FALSE(result.stable);
}

TEST(StabilityTests, CoefficientLessThanMinusOne) {
    auto result = StabilityAnalyzer::analyze(-1.5);

    EXPECT_FALSE(result.stable);
}

// Проверка некорректных числовых значений

TEST(StabilityTests, NaNValue) {
    auto result = StabilityAnalyzer::analyze(
        std::numeric_limits<double>::quiet_NaN()
    );

    EXPECT_FALSE(result.stable);
}

TEST(StabilityTests, InfiniteValue) {
    auto result = StabilityAnalyzer::analyze(
        std::numeric_limits<double>::infinity()
    );

    EXPECT_FALSE(result.stable);
}

TEST(StabilityTests, NegativeInfiniteValue) {
    auto result = StabilityAnalyzer::analyze(
        -std::numeric_limits<double>::infinity()
    );

    EXPECT_FALSE(result.stable);
}
// Тесты модели 1.10
TEST(ModelTests, DistributedControlFirstStep) {
    DistributedControlModel model(0.5, 0.2, 0.1, 0.0);

    model.nextStep(1.0);

    EXPECT_DOUBLE_EQ(model.y(), 0.2);
}

TEST(ModelTests, DistributedControlReset) {
    DistributedControlModel model(0.5, 0.2, 0.1, 1.0);

    model.nextStep(1.0);
    model.reset();

    EXPECT_DOUBLE_EQ(model.y(), 1.0);
}

// Тесты модели 2.2
TEST(ModelTests, SaturationUpperLimit) {
    SaturationModel model(0.5, 2.0, -1.0, 1.0, 0.0);

    model.nextStep(3.0);

    EXPECT_DOUBLE_EQ(model.y(), 2.0);
}

TEST(ModelTests, SaturationLowerLimit) {
    SaturationModel model(0.5, 2.0, -1.0, 1.0, 0.0);

    model.nextStep(-3.0);

    EXPECT_DOUBLE_EQ(model.y(), -2.0);
}

// Тесты модели 3.4
TEST(ModelTests, QuadraticDecayFirstStep) {
    QuadraticDecayModel model(1.0, 0.1, 1.0);

    model.nextStep(0.0);

    EXPECT_DOUBLE_EQ(model.y(), 0.9);
}

// Параметризованные тесты модели насыщения
class SaturationModelTest
    : public ::testing::TestWithParam<double> {
};

TEST_P(SaturationModelTest, ChecksUpperSaturation) {
    double input = GetParam();

    SaturationModel model(0.5, 2.0, -1.0, 1.0, 0.0);

    model.nextStep(input);

    EXPECT_LE(model.y(), 2.0);
    EXPECT_GE(model.y(), -2.0);
}

INSTANTIATE_TEST_SUITE_P(
    DifferentInputs,
    SaturationModelTest,
    ::testing::Values(-10.0, -3.0, 0.0, 0.5, 3.0, 10.0)
);

// Mock-тест ступенчатого воздействия
TEST(MockInputTests, StepInput) {
    MockInput input;
    DistributedControlModel model(0.0, 1.0, 0.0, 0.0);

    EXPECT_CALL(input, value(0))
        .WillOnce(::testing::Return(1.0));

    model.nextStep(input.value(0));

    EXPECT_DOUBLE_EQ(model.y(), 1.0);
}

// Mock-тест импульсного воздействия
TEST(MockInputTests, ImpulseInput) {
    MockInput input;
    DistributedControlModel model(0.0, 1.0, 0.0, 0.0);

    EXPECT_CALL(input, value(0))
        .WillOnce(::testing::Return(1.0));

    EXPECT_CALL(input, value(1))
        .WillOnce(::testing::Return(0.0));

    model.nextStep(input.value(0));
    model.nextStep(input.value(1));

    EXPECT_DOUBLE_EQ(model.y(), 0.0);
}

// Mock-тест гармонического воздействия
TEST(MockInputTests, HarmonicInput) {
    MockInput input;
    DistributedControlModel model(0.0, 1.0, 0.0, 0.0);

    EXPECT_CALL(input, value(1))
        .WillOnce(::testing::Return(0.8414709848));

    model.nextStep(input.value(1));

    EXPECT_NEAR(model.y(), 0.8414709848, 0.000001);
}

// Граничные тесты модели 1.10
TEST(ModelBoundaryTests, DistributedControlTwoStepDelay) {
    DistributedControlModel model(0.0, 0.0, 1.0, 0.0);

    model.nextStep(1.0);
    model.nextStep(2.0);
    model.nextStep(3.0);

    EXPECT_DOUBLE_EQ(model.y(), 1.0);
}

// Граничные тесты модели 2.2
TEST(ModelBoundaryTests, SaturationAtUpperBoundary) {
    SaturationModel model(0.0, 1.0, -1.0, 1.0, 0.0);

    model.nextStep(1.0);

    EXPECT_DOUBLE_EQ(model.y(), 1.0);
}

TEST(ModelBoundaryTests, SaturationAtLowerBoundary) {
    SaturationModel model(0.0, 1.0, -1.0, 1.0, 0.0);

    model.nextStep(-1.0);

    EXPECT_DOUBLE_EQ(model.y(), -1.0);
}

// Граничные тесты модели 3.4
TEST(ModelBoundaryTests, QuadraticDecayZeroInitialValue) {
    QuadraticDecayModel model(1.0, 0.1, 0.0);

    model.nextStep(0.0);

    EXPECT_DOUBLE_EQ(model.y(), 0.0);
}

TEST(ModelBoundaryTests, QuadraticDecayZeroCoefficient) {
    QuadraticDecayModel model(0.0, 0.1, 5.0);

    model.nextStep(0.0);

    EXPECT_DOUBLE_EQ(model.y(), 5.0);
}

TEST(ModelBoundaryTests, QuadraticDecayZeroStep) {
    QuadraticDecayModel model(1.0, 0.0, 5.0);

    model.nextStep(0.0);

    EXPECT_DOUBLE_EQ(model.y(), 5.0);
}

TEST(ModelNameTests, DistributedControlModelName) {
    DistributedControlModel model(0.7, 0.5, 0.3);
    EXPECT_EQ(model.name(), "Model_1_10_DistributedControl");
}

TEST(ModelNameTests, SaturationModelName) {
    SaturationModel model(0.7, 0.5, -1.0, 1.0);
    EXPECT_EQ(model.name(), "Model_2_2_Saturation");
}

TEST(ModelNameTests, QuadraticDecayModelName) {
    QuadraticDecayModel model(0.1, 0.01, 1.0);
    EXPECT_EQ(model.name(), "Model_3_4_QuadraticDecay");
}