#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "Simulation.h"
#include "ApplicationLogic.h"
#include <cmath>

using ::testing::Return;
using ::testing::_;

// ========================================================
// 1. БАЗОВЫЙ КЛАСС MODEL (Геттеры, Сеттеры, Сброс)
// ========================================================
TEST(BaseModelTest, StateGetSetReset) {
    Model1_1 m(0.5, 1.0, 10.0);
    EXPECT_DOUBLE_EQ(m.getState(), 10.0);
    EXPECT_DOUBLE_EQ(m.getY(), 10.0);

    m.setY(25.5);
    EXPECT_DOUBLE_EQ(m.getState(), 25.5);

    m.reset(0.0);
    EXPECT_DOUBLE_EQ(m.getState(), 0.0);

    m.reset(5.0);
    EXPECT_DOUBLE_EQ(m.getState(), 5.0);
}

// ========================================================
// 2. ПАРАМЕТРИЗОВАННЫЕ ТЕСТЫ ДЛЯ ЛИНЕЙНОЙ МОДЕЛИ (Model1_1)
// ========================================================
struct LinearParam {
    double a;
    double b;
    double y0;
    double u;
    double expectedY1;
    double expectedY2;
};

class LinearParamTest : public ::testing::TestWithParam<LinearParam> {};

TEST_P(LinearParamTest, TwoStepsVerification) {
    const auto& p = GetParam();
    Model1_1 model(p.a, p.b, p.y0);

    double y1 = model.step(p.u);
    EXPECT_NEAR(y1, p.expectedY1, 1e-5);

    double y2 = model.step(p.u);
    EXPECT_NEAR(y2, p.expectedY2, 1e-5);
}

INSTANTIATE_TEST_SUITE_P(
    LinearExhaustive,
    LinearParamTest,
    ::testing::Values(
        LinearParam{0.5, 1.0, 0.0, 0.0, 0.0, 0.0},
        LinearParam{0.0, 1.0, 0.0, 0.0, 0.0, 0.0},
        LinearParam{0.0, 2.0, 0.0, 1.5, 3.0, 3.0},
        LinearParam{0.0, -2.0, 0.0, 1.5, -3.0, -3.0},
        LinearParam{0.5, 1.0, 0.0, 1.0, 1.0, 1.5},
        LinearParam{0.5, 2.0, 0.0, 1.0, 2.0, 3.0},
        LinearParam{0.8, 1.0, 0.0, 2.0, 2.0, 3.6},
        LinearParam{0.2, 0.5, 0.0, 4.0, 2.0, 2.4},
        LinearParam{0.5, 1.0, 4.0, 1.0, 3.0, 2.5},
        LinearParam{0.8, 1.0, 5.0, 1.0, 5.0, 5.0},
        LinearParam{0.2, 2.0, 10.0, 0.0, 2.0, 0.4},
        LinearParam{-0.5, 1.0, 2.0, 1.0, 0.0, 1.0},
        LinearParam{-0.4, 2.0, 1.0, 2.0, 3.6, 2.56},
        LinearParam{0.5, -1.0, 0.0, 2.0, -2.0, -3.0},
        LinearParam{0.5, 1.0, 0.0, -2.0, -2.0, -3.0},
        LinearParam{-0.5, -1.0, 0.0, -2.0, 2.0, 1.0}
    )
);

// ========================================================
// 3. ПАРАМЕТРИЗОВАННЫЕ ТЕСТЫ ДЛЯ Model2_5 (Нелинейность)
// ========================================================
struct NonLinearParam {
    double a;
    double b;
    double y0;
    double u;
    double expectedY1;
};

class NonLinearParamTest : public ::testing::TestWithParam<NonLinearParam> {};

TEST_P(NonLinearParamTest, StepCalculation) {
    const auto& p = GetParam();
    Model2_5 model(p.a, p.b, p.y0);
    double y1 = model.step(p.u);
    EXPECT_NEAR(y1, p.expectedY1, 1e-4);
}

INSTANTIATE_TEST_SUITE_P(
    NonLinearExhaustive,
    NonLinearParamTest,
    ::testing::Values(
        NonLinearParam{0.5, 2.0, 0.0, 0.0, 0.0},
        NonLinearParam{0.0, 2.0, 0.0, 1.0, 1.26424},
        NonLinearParam{0.0, 2.0, 0.0, -1.0, -1.26424},
        NonLinearParam{0.0, 2.0, 0.0, 2.0, 1.72933},
        NonLinearParam{0.0, 2.0, 0.0, 0.5, 0.78694},
        NonLinearParam{0.5, 2.0, 2.0, 1.0, 2.26424},
        NonLinearParam{0.0, 5.0, 0.0, 10.0, 4.9997},
        NonLinearParam{0.0, 5.0, 0.0, -10.0, -4.9997},
        NonLinearParam{0.0, 5.0, 0.0, 50.0, 5.0000},
        NonLinearParam{0.0, 5.0, 0.0, -50.0, -5.0000}
    )
);

TEST(Model2_5SpecialTest, SignFunctionValues) {
    EXPECT_DOUBLE_EQ(Model2_5::sign(10.5), 1.0);
    EXPECT_DOUBLE_EQ(Model2_5::sign(-5.2), -1.0);
    EXPECT_DOUBLE_EQ(Model2_5::sign(0.0), 0.0);
}

// ========================================================
// 4. ПАРАМЕТРИЗОВАННЫЕ ТЕСТЫ ДЛЯ Model3_9 (Метод Эйлера)
// ========================================================
struct EulerParam {
    double b;
    double dt;
    double y0;
    double u;
    double expectedY1;
};

class EulerParamTest : public ::testing::TestWithParam<EulerParam> {};

TEST_P(EulerParamTest, StepCalculation) {
    const auto& p = GetParam();
    Model3_9 model(p.b, p.dt, p.y0);
    double y1 = model.step(p.u);
    EXPECT_NEAR(y1, p.expectedY1, 1e-4);
}

INSTANTIATE_TEST_SUITE_P(
    EulerExhaustive,
    EulerParamTest,
    ::testing::Values(
        EulerParam{1.0, 0.1, 0.0, 0.0, 0.0},
        EulerParam{1.0, 0.1, 0.0, 1.0, 0.07616},
        EulerParam{1.0, 0.1, 0.0, -1.0, -0.07616},
        EulerParam{2.0, 0.05, 0.0, 1.0, 0.07616},
        EulerParam{2.0, 0.1, 5.0, 0.5, 5.09242},
        EulerParam{1.0, 0.2, 0.0, 10.0, 0.2},
        EulerParam{1.0, 0.2, 0.0, -10.0, -0.2},
        EulerParam{3.0, 0.1, 1.0, 5.0, 1.3}
    )
);

// ========================================================
// 5. GOOGLE MOCK: ТЕСТИРОВАНИЕ РЕЖИМОВ УПРАВЛЕНИЯ
// ========================================================
class MockInputSignal : public IInputSignal {
public:
    MOCK_METHOD(double, getValue, (int step), (override));
};

TEST(MockSignalTest, StepSignalSimulation) {
    MockInputSignal mockSignal;
    EXPECT_CALL(mockSignal, getValue(_)).WillRepeatedly(Return(1.0));

    Model1_1 model(0.5, 1.0, 0.0);
    auto res = ApplicationLogic::simulate(model, mockSignal, 4);

    ASSERT_EQ(res.size(), 4);
    EXPECT_NEAR(res[0], 1.0, 1e-5);
    EXPECT_NEAR(res[1], 1.5, 1e-5);
    EXPECT_NEAR(res[2], 1.75, 1e-5);
    EXPECT_NEAR(res[3], 1.875, 1e-5);
}

TEST(MockSignalTest, ImpulseSignalSimulation) {
    MockInputSignal mockSignal;
    EXPECT_CALL(mockSignal, getValue(0)).WillOnce(Return(2.0));
    EXPECT_CALL(mockSignal, getValue(1)).WillOnce(Return(0.0));
    EXPECT_CALL(mockSignal, getValue(2)).WillOnce(Return(0.0));
    EXPECT_CALL(mockSignal, getValue(3)).WillOnce(Return(0.0));

    Model1_1 model(0.5, 1.0, 0.0);
    auto res = ApplicationLogic::simulate(model, mockSignal, 4);

    ASSERT_EQ(res.size(), 4);
    EXPECT_NEAR(res[0], 2.0, 1e-5);
    EXPECT_NEAR(res[1], 1.0, 1e-5);
    EXPECT_NEAR(res[2], 0.5, 1e-5);
    EXPECT_NEAR(res[3], 0.25, 1e-5);
}

TEST(MockSignalTest, HarmonicAlternatingSignalSimulation) {
    MockInputSignal mockSignal;
    EXPECT_CALL(mockSignal, getValue(0)).WillOnce(Return(1.0));
    EXPECT_CALL(mockSignal, getValue(1)).WillOnce(Return(-1.0));
    EXPECT_CALL(mockSignal, getValue(2)).WillOnce(Return(1.0));

    Model1_1 model(0.5, 1.0, 0.0);
    auto res = ApplicationLogic::simulate(model, mockSignal, 3);

    ASSERT_EQ(res.size(), 3);
    EXPECT_NEAR(res[0], 1.0, 1e-5);
    EXPECT_NEAR(res[1], -0.5, 1e-5);
    EXPECT_NEAR(res[2], 0.75, 1e-5);
}

TEST(MockSignalTest, EmptySimulationZeroSteps) {
    MockInputSignal mockSignal;
    EXPECT_CALL(mockSignal, getValue(_)).Times(0);

    Model1_1 model(0.5, 1.0, 0.0);
    auto res = ApplicationLogic::simulate(model, mockSignal, 0);

    EXPECT_TRUE(res.empty());
}