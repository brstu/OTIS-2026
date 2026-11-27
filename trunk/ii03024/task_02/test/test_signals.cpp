// Тесты входных воздействий и режимов управления (в т.ч. с Google Mock).
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <cmath>

#include "InputSignal.h"
#include "Simulator.h"
#include "model11.h"

using ::testing::DoAll;
using ::testing::InSequence;
using ::testing::Return;
using ::testing::SaveArg;

namespace {

class MockSignal : public ISignal {
public:
    MOCK_METHOD(double, value, (int tau), (const, override));
};

class MockModel : public Model {
public:
    MOCK_METHOD(double, nextStep, (double u, double dt), (override));
    MOCK_METHOD(std::string, getName, (), (const, override));
};

}  // namespace

// ---------- Реальные сигналы ----------

TEST(Signals, StepIsConstant) {
    StepSignal s(2.5);
    for (int t = 0; t < 10; ++t) EXPECT_DOUBLE_EQ(s.value(t), 2.5);
}

TEST(Signals, PulseOnlyAtFirstStep) {
    PulseSignal s(3.0);
    EXPECT_DOUBLE_EQ(s.value(0), 3.0);
    for (int t = 1; t < 10; ++t) EXPECT_DOUBLE_EQ(s.value(t), 0.0);
}

TEST(Signals, HarmonicDefaultIsSinTau) {
    HarmonicSignal s(2.0);
    EXPECT_DOUBLE_EQ(s.value(0), 0.0);
    EXPECT_NEAR(s.value(1), 2.0 * std::sin(1.0), 1e-12);
    EXPECT_NEAR(s.value(3), 2.0 * std::sin(3.0), 1e-12);
}

TEST(Signals, HarmonicWithPhaseStep) {
    HarmonicSignal s(1.0, 0.1);
    EXPECT_NEAR(s.value(5), std::sin(0.5), 1e-12);
}

// ---------- Simulator ----------

TEST(Simulator, ReturnsNPlusOneSamples) {
    Model11 m(0.5, 1.0);
    StepSignal s(1.0);
    const auto r = simulate(m, s, 7, 0.1);
    ASSERT_EQ(r.size(), 8u);
    EXPECT_EQ(r.front().tau, 0);
    EXPECT_EQ(r.back().tau, 7);
}

TEST(Simulator, NegativeNGivesEmptyResult) {
    Model11 m(0.5, 1.0);
    StepSignal s(1.0);
    EXPECT_TRUE(simulate(m, s, -1, 0.1).empty());
}

// ---------- Google Mock: подмена входного сигнала ----------

TEST(MockedSignal, StepModeDrivesRealModel) {
    MockSignal sig;
    EXPECT_CALL(sig, value(::testing::_)).Times(3).WillRepeatedly(Return(1.0));   // "ступенька"
    Model11 m(0.5, 2.0);
    const auto r = simulate(m, sig, 2, 0.1);
    EXPECT_NEAR(r[0].y, 2.0, 1e-12);
    EXPECT_NEAR(r[1].y, 3.0, 1e-12);
    EXPECT_NEAR(r[2].y, 3.5, 1e-12);
}

TEST(MockedSignal, PulseModeDrivesRealModel) {
    MockSignal sig;
    InSequence seq;
    EXPECT_CALL(sig, value(0)).WillOnce(Return(1.0));
    EXPECT_CALL(sig, value(1)).WillOnce(Return(0.0));
    EXPECT_CALL(sig, value(2)).WillOnce(Return(0.0));
    Model11 m(0.5, 1.0);
    const auto r = simulate(m, sig, 2, 0.1);
    EXPECT_NEAR(r[0].y, 1.0, 1e-12);
    EXPECT_NEAR(r[1].y, 0.5, 1e-12);
    EXPECT_NEAR(r[2].y, 0.25, 1e-12);
}

TEST(MockedSignal, HarmonicModeValuesAreRecorded) {
    MockSignal sig;
    EXPECT_CALL(sig, value(0)).WillOnce(Return(0.0));
    EXPECT_CALL(sig, value(1)).WillOnce(Return(0.8));
    EXPECT_CALL(sig, value(2)).WillOnce(Return(-0.8));
    Model11 m(0.0, 1.0);
    const auto r = simulate(m, sig, 2, 0.1);
    EXPECT_DOUBLE_EQ(r[0].u, 0.0);
    EXPECT_DOUBLE_EQ(r[1].u, 0.8);
    EXPECT_DOUBLE_EQ(r[2].y, -0.8);
}

// ---------- Google Mock: подмена модели ----------

TEST(MockedModel, SimulatorPassesSignalAndStepToModel) {
    MockModel model;
    StepSignal sig(4.0);
    InSequence seq;
    EXPECT_CALL(model, nextStep(4.0, 0.25)).WillOnce(Return(10.0));
    EXPECT_CALL(model, nextStep(4.0, 0.25)).WillOnce(Return(20.0));

    const auto r = simulate(model, sig, 1, 0.25);
    ASSERT_EQ(r.size(), 2u);
    EXPECT_DOUBLE_EQ(r[0].y, 10.0);
    EXPECT_DOUBLE_EQ(r[1].y, 20.0);
    EXPECT_DOUBLE_EQ(r[1].u, 4.0);
}

TEST(MockedModel, ModelNotCalledForNegativeN) {
    MockModel model;
    StepSignal sig(1.0);
    EXPECT_CALL(model, nextStep(::testing::_, ::testing::_)).Times(0);
    EXPECT_TRUE(simulate(model, sig, -5, 0.1).empty());
}
