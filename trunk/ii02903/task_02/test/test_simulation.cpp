// Tests of simulate() and of the control modes (step, impulse, harmonic).
// Google Mock objects replace the input signal and the model.
#include <cmath>
#include <memory>
#include <stdexcept>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "mocks.h"
#include "models.h"
#include "signals.h"
#include "simulation.h"

using ::testing::_;
using ::testing::DoubleEq;
using ::testing::InSequence;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::StrictMock;

// ---------- the shape of the result ----------
TEST(Simulate, ReturnsNPlusOneRowsWithTimeAndInitialValue) {
    DiffModel37 model(0.5, 2.0, 0.1, 0.7);
    const StepInput input(1.0);
    const std::vector<Row> rows = simulate(model, input, 5);

    ASSERT_EQ(rows.size(), 6u);
    EXPECT_EQ(rows[0].tau, 0);
    EXPECT_DOUBLE_EQ(rows[0].y, 0.7);  // the first row holds y(0)
    for (size_t i = 0; i < rows.size(); ++i) {
        EXPECT_EQ(rows[i].tau, static_cast<int>(i));
        EXPECT_NEAR(rows[i].t, static_cast<double>(i) * 0.1, 1e-12);
    }
}

TEST(Simulate, ZeroStepsReturnsOnlyTheInitialRow) {
    StrictMock<MockModel> model;  // nextStep() must not be called at all
    NiceMock<MockInputSignal> input;
    EXPECT_CALL(model, timeStep()).WillRepeatedly(Return(1.0));
    EXPECT_CALL(model, current()).WillOnce(Return(4.0));
    EXPECT_CALL(input, value(0, _)).WillOnce(Return(2.0));

    const std::vector<Row> rows = simulate(model, input, 0);
    ASSERT_EQ(rows.size(), 1u);
    EXPECT_DOUBLE_EQ(rows[0].y, 4.0);
    EXPECT_DOUBLE_EQ(rows[0].u, 2.0);
}

TEST(Simulate, NegativeNumberOfStepsIsRejected) {
    LinearModel13 model(0.5, 0.3, 1.0, 0.0);
    const StepInput input(1.0);
    EXPECT_THROW(simulate(model, input, -1), std::invalid_argument);
}

// ---------- mock signal: how the signal is asked ----------
// The signal must be asked for tau = 0, 1, 2, 3 and for the time t = tau * dt.
TEST(Simulate, AsksTheSignalForEveryStepWithTimeTauTimesDt) {
    NiceMock<MockModel> model;
    NiceMock<MockInputSignal> input;
    ON_CALL(model, timeStep()).WillByDefault(Return(0.5));

    EXPECT_CALL(input, value(0, DoubleEq(0.0))).WillOnce(Return(1.0));
    EXPECT_CALL(input, value(1, DoubleEq(0.5))).WillOnce(Return(1.0));
    EXPECT_CALL(input, value(2, DoubleEq(1.0))).WillOnce(Return(1.0));
    EXPECT_CALL(input, value(3, DoubleEq(1.5))).WillOnce(Return(1.0));

    simulate(model, input, 3);
}

// ---------- mock model: which values the model receives ----------
// n = 3 steps: nextStep() is called 3 times (for tau = 0, 1, 2), the last row only reads y(3).
TEST(Simulate, PassesSignalValuesToTheModelExactlyNTimes) {
    NiceMock<MockModel> model;
    NiceMock<MockInputSignal> input;
    EXPECT_CALL(input, value(0, _)).WillOnce(Return(1.5));
    EXPECT_CALL(input, value(1, _)).WillOnce(Return(-2.0));
    EXPECT_CALL(input, value(2, _)).WillOnce(Return(0.25));
    EXPECT_CALL(input, value(3, _)).WillOnce(Return(99.0));  // is read, but never given to the model

    {
        InSequence order;
        EXPECT_CALL(model, nextStep(1.5));
        EXPECT_CALL(model, nextStep(-2.0));
        EXPECT_CALL(model, nextStep(0.25));
    }
    EXPECT_CALL(model, nextStep(99.0)).Times(0);

    const std::vector<Row> rows = simulate(model, input, 3);
    ASSERT_EQ(rows.size(), 4u);
    EXPECT_DOUBLE_EQ(rows[3].u, 99.0);
}

// ---------- control mode: impulse (mock signal returns 1, 0, 0, 0) ----------
TEST(Simulate, ImpulseModeWithMockSignal) {
    LinearModel13 model(0.5, 0.3, 1.0, 0.0);
    NiceMock<MockInputSignal> input;
    EXPECT_CALL(input, value(0, _)).WillOnce(Return(1.0));
    EXPECT_CALL(input, value(::testing::Gt(0), _)).WillRepeatedly(Return(0.0));

    const std::vector<Row> rows = simulate(model, input, 4);
    EXPECT_NEAR(rows[1].y, 1.0, 1e-12);
    EXPECT_NEAR(rows[2].y, 0.5, 1e-12);
    EXPECT_NEAR(rows[3].y, 0.55, 1e-12);
    EXPECT_NEAR(rows[4].y, 0.425, 1e-12);
}

// ---------- control mode: step (mock signal gives the same result as StepInput) ----------
TEST(Simulate, MockConstantSignalGivesTheSameResultAsStepInput) {
    NiceMock<MockInputSignal> mock;
    ON_CALL(mock, value(_, _)).WillByDefault(Return(1.0));
    const StepInput real(1.0);

    LinearModel13 m1(0.5, 0.3, 1.0, 0.0);
    LinearModel13 m2(0.5, 0.3, 1.0, 0.0);
    const std::vector<Row> a = simulate(m1, mock, 20);
    const std::vector<Row> b = simulate(m2, real, 20);
    ASSERT_EQ(a.size(), b.size());
    for (size_t i = 0; i < a.size(); ++i) EXPECT_DOUBLE_EQ(a[i].y, b[i].y);
}

// ---------- end to end with the real signals ----------
TEST(Simulate, StepResponseOfModel13ReachesSteadyStateFive) {
    LinearModel13 model(0.5, 0.3, 1.0, 0.0);
    const StepInput input(1.0);
    const std::vector<Row> rows = simulate(model, input, 200);
    EXPECT_NEAR(rows.back().y, 1.0 / (1.0 - 0.5 - 0.3), 1e-9);
}

TEST(Simulate, ImpulseResponseOfStableModelDecaysToZero) {
    LinearModel13 model(0.5, 0.3, 1.0, 0.0);
    const ImpulseInput input;
    const std::vector<Row> rows = simulate(model, input, 300);  // 0.85^300 is about 1e-21
    EXPECT_NEAR(rows.back().y, 0.0, 1e-9);
}

TEST(Simulate, HarmonicInputOfModel25StaysBounded) {
    // |sin| <= 1 and the added term is at most b, so |y| <= b / (1 - a)
    NonlinearModel25 model(0.5, 2.0, 0.0);
    const HarmonicInput input;
    const std::vector<Row> rows = simulate(model, input, 200);
    for (const Row& r : rows) EXPECT_LE(std::fabs(r.y), 2.0 / (1.0 - 0.5) + 1e-9);
}

// ---------- parameterized: all three control modes ----------
// 0 - step, 1 - impulse, 2 - harmonic
class ControlModeTest : public ::testing::TestWithParam<int> {
protected:
    static std::unique_ptr<InputSignal> makeSignal(int mode) {
        if (mode == 0) return std::make_unique<StepInput>(1.5);
        if (mode == 1) return std::make_unique<ImpulseInput>();
        return std::make_unique<HarmonicInput>();
    }
};

TEST_P(ControlModeTest, RowsContainTheSignalValuesForEveryModel) {
    const std::unique_ptr<InputSignal> input = makeSignal(GetParam());

    LinearModel13 m13(0.5, 0.3, 1.0, 0.0);
    NonlinearModel25 m25(0.5, 2.0, 0.0);
    DiffModel37 m37(0.5, 2.0, 0.1, 0.0);
    Model* models[] = {&m13, &m25, &m37};

    for (Model* model : models) {
        const double dt = model->timeStep();
        const std::vector<Row> rows = simulate(*model, *input, 15);
        ASSERT_EQ(rows.size(), 16u) << model->name();
        for (const Row& r : rows) {
            EXPECT_DOUBLE_EQ(r.u, input->value(r.tau, r.tau * dt)) << model->name();
            EXPECT_TRUE(std::isfinite(r.y)) << model->name();
        }
    }
}

std::string modeName(const ::testing::TestParamInfo<int>& info) {
    if (info.param == 0) return "Step";
    if (info.param == 1) return "Impulse";
    return "Harmonic";
}

INSTANTIATE_TEST_SUITE_P(Modes, ControlModeTest, ::testing::Values(0, 1, 2), modeName);
