// Google Mock: подмена входных сигналов (ступень, импульс, гармоника) и подмена модели.
#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <cmath>
#include <sstream>
#include <stdexcept>
#include <string>

#include "InputSignals.h"
#include "Model1_7.h"
#include "Simulation.h"

using ::testing::_;
using ::testing::DoAll;
using ::testing::InSequence;
using ::testing::Invoke;
using ::testing::Return;
using ::testing::StrictMock;

class MockInputSignal : public InputSignal {
public:
    MOCK_METHOD(double, value, (int tau), (const, override));
    MOCK_METHOD(std::string, name, (), (const, override));
};

class MockModel : public Model {
public:
    MOCK_METHOD(double, next, (double u), (override));
    MOCK_METHOD(void, reset, (), (override));
    MOCK_METHOD(std::string, name, (), (const, override));
    MOCK_METHOD(bool, isStable, (), (const, override));
    MOCK_METHOD(std::string, stabilityWarning, (), (const, override));
};

// ---------- подмена сигнала: simulate запрашивает tau = 1..n по порядку ----------
TEST(SimulateWithMockSignal, RequestsEachTauExactlyOnceInOrder) {
    StrictMock<MockInputSignal> sig;
    Model1_7 model(0.5, 0.4, 0.2, 0.1);
    {
        InSequence seq;
        EXPECT_CALL(sig, value(1)).WillOnce(Return(0.0));
        EXPECT_CALL(sig, value(2)).WillOnce(Return(0.0));
        EXPECT_CALL(sig, value(3)).WillOnce(Return(0.0));
        EXPECT_CALL(sig, value(4)).WillOnce(Return(0.0));
    }
    const auto steps = simulate(model, sig, 4);
    EXPECT_EQ(steps.size(), 4u);
}

TEST(SimulateWithMockSignal, StepMode) {
    MockInputSignal sig;
    EXPECT_CALL(sig, value(_)).Times(4).WillRepeatedly(Return(1.0));
    Model1_7 model(0.5, 0.4, 0.2, 0.1);

    const auto steps = simulate(model, sig, 4);

    const double expected[] = {0.4, 0.8, 1.1, 1.25};
    for (int i = 0; i < 4; ++i) {
        EXPECT_DOUBLE_EQ(steps[i].u, 1.0);
        EXPECT_NEAR(steps[i].y, expected[i], 1e-12);
    }
}

TEST(SimulateWithMockSignal, ImpulseMode) {
    MockInputSignal sig;
    EXPECT_CALL(sig, value(1)).WillOnce(Return(1.0));
    EXPECT_CALL(sig, value(2)).WillOnce(Return(0.0));
    EXPECT_CALL(sig, value(3)).WillOnce(Return(0.0));
    EXPECT_CALL(sig, value(4)).WillOnce(Return(0.0));
    Model1_7 model(0.5, 0.4, 0.2, 0.1);

    const auto steps = simulate(model, sig, 4);

    const double expected[] = {0.4, 0.4, 0.3, 0.15};
    for (int i = 0; i < 4; ++i) EXPECT_NEAR(steps[i].y, expected[i], 1e-12);
}

TEST(SimulateWithMockSignal, HarmonicModeEqualsRealHarmonicInput) {
    MockInputSignal sig;
    EXPECT_CALL(sig, value(_)).Times(6).WillRepeatedly(Invoke([](int tau) {
        return std::sin(static_cast<double>(tau));
    }));
    Model1_7 mocked(0.5, 0.4, 0.2, 0.1), real(0.5, 0.4, 0.2, 0.1);
    HarmonicInput harmonic;

    const auto a = simulate(mocked, sig, 6);
    const auto b = simulate(real, harmonic, 6);

    ASSERT_EQ(a.size(), b.size());
    for (std::size_t i = 0; i < a.size(); ++i) {
        EXPECT_DOUBLE_EQ(a[i].u, b[i].u);
        EXPECT_DOUBLE_EQ(a[i].y, b[i].y);
    }
}

// ---------- подмена модели ----------
TEST(SimulateWithMockModel, ResetsBeforeFirstStepAndPassesInputs) {
    MockInputSignal sig;
    StrictMock<MockModel> model;
    EXPECT_CALL(sig, value(1)).WillOnce(Return(2.0));
    EXPECT_CALL(sig, value(2)).WillOnce(Return(-3.0));
    {
        InSequence seq;
        EXPECT_CALL(model, reset());
        EXPECT_CALL(model, next(2.0)).WillOnce(Return(10.0));
        EXPECT_CALL(model, next(-3.0)).WillOnce(Return(20.0));
    }

    const auto steps = simulate(model, sig, 2);

    ASSERT_EQ(steps.size(), 2u);
    EXPECT_EQ(steps[0].tau, 1);
    EXPECT_DOUBLE_EQ(steps[0].u, 2.0);
    EXPECT_DOUBLE_EQ(steps[0].y, 10.0);
    EXPECT_EQ(steps[1].tau, 2);
    EXPECT_DOUBLE_EQ(steps[1].u, -3.0);
    EXPECT_DOUBLE_EQ(steps[1].y, 20.0);
}

TEST(SimulateWithMockModel, InvalidStepCountTouchesNothing) {
    StrictMock<MockInputSignal> sig;
    StrictMock<MockModel> model;  // любой вызов = провал теста
    EXPECT_THROW(simulate(model, sig, 0), std::invalid_argument);
    EXPECT_THROW(simulate(model, sig, -5), std::invalid_argument);
}

// ---------- предупреждение об устойчивости (выводится до симуляции) ----------
TEST(StabilityWarningOutput, StableModelPrintsNothing) {
    StrictMock<MockModel> model;
    EXPECT_CALL(model, isStable()).WillOnce(Return(true));
    std::ostringstream out;

    EXPECT_FALSE(printStabilityWarning(model, out));
    EXPECT_TRUE(out.str().empty());
}

TEST(StabilityWarningOutput, UnstableModelPrintsItsWarning) {
    StrictMock<MockModel> model;
    EXPECT_CALL(model, isStable()).WillOnce(Return(false));
    EXPECT_CALL(model, stabilityWarning()).WillOnce(Return("[WARN] test text"));
    std::ostringstream out;

    EXPECT_TRUE(printStabilityWarning(model, out));
    EXPECT_NE(out.str().find("WARNING"), std::string::npos);
    EXPECT_NE(out.str().find("[WARN] test text"), std::string::npos);
}

TEST(StabilityWarningOutput, RealUnstableModelWarnsBeforeSimulation) {
    Model1_7 model(1.5, 0.4, 0.2, 0.1);
    std::ostringstream out;
    EXPECT_TRUE(printStabilityWarning(model, out));
    EXPECT_NE(out.str().find("UNSTABLE"), std::string::npos);
}

// ---------- контракт базового класса и исключения ----------
namespace {
// Минимальная модель: не переопределяет isStable/stabilityWarning
class BareModel : public Model {
public:
    double next(double u) override { return u; }
    void reset() override {}
    std::string name() const override { return "Bare"; }
};
}  // namespace

TEST(ModelBaseClass, DefaultsMeanStableWithoutWarning) {
    const BareModel m;
    EXPECT_TRUE(m.isStable());
    EXPECT_EQ(m.stabilityWarning(), "");
}

TEST(SimulateWithMockModel, ExceptionFromModelPropagates) {
    MockInputSignal sig;
    StrictMock<MockModel> model;
    EXPECT_CALL(sig, value(_)).WillRepeatedly(Return(1.0));
    EXPECT_CALL(model, reset());
    EXPECT_CALL(model, next(_)).WillOnce(Return(1.0)).WillOnce(::testing::Throw(std::runtime_error("boom")));

    EXPECT_THROW(simulate(model, sig, 5), std::runtime_error);
}
