#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <cmath>
#include <vector>

#include "SignalSource.h"
#include "Simulation.h"
#include "TestUtils.h"

using ::testing::NiceMock;
using ::testing::Return;

TEST(SignalSource, StepAndImpulse)
{
    {
        SCOPED_TRACE("ступенька постоянна");
        EXPECT_DOUBLE_EQ(sampleSignal(InputKind::Step, 0, 2.5), 2.5);
        EXPECT_DOUBLE_EQ(sampleSignal(InputKind::Step, 1, 2.5), 2.5);
        EXPECT_DOUBLE_EQ(sampleSignal(InputKind::Step, 100, 2.5), 2.5);
    }
    {
        SCOPED_TRACE("импульс ненулевой только на первом такте");
        EXPECT_DOUBLE_EQ(sampleSignal(InputKind::Impulse, 0, 1.7), 1.7);
        EXPECT_DOUBLE_EQ(sampleSignal(InputKind::Impulse, 1, 1.7), 0.0);
        EXPECT_DOUBLE_EQ(sampleSignal(InputKind::Impulse, 50, 1.7), 0.0);
    }
}

TEST(SignalSource, HarmonicFollowsSine)
{
    const double amplitude = 3.0;
    for (int tau = 0; tau <= 10; ++tau)
        EXPECT_NEAR(sampleSignal(InputKind::Harmonic, tau, amplitude),
                    amplitude * std::sin(static_cast<double>(tau)), 1e-12);

    EXPECT_NEAR(sampleSignal(InputKind::Harmonic, 0, 5.0), 0.0, 1e-15);

    const double shift = 2.0 * 3.14159265358979323846;
    for (int tau = 0; tau <= 5; ++tau)
        EXPECT_NEAR(sampleSignal(InputKind::Harmonic, tau, 1.5),
                    1.5 * std::sin(tau + shift), 1e-12);
}

TEST(SignalSource, ZeroNegativeAndInvalidAmplitude)
{
    {
        SCOPED_TRACE("нулевая амплитуда дает ноль");
        for (int type = 1; type <= 3; ++type)
        {
            const auto signal = static_cast<InputKind>(type);
            for (int tau = 0; tau <= 5; ++tau)
                EXPECT_DOUBLE_EQ(sampleSignal(signal, tau, 0.0), 0.0);
        }
    }
    {
        SCOPED_TRACE("отрицательная амплитуда инвертирует сигнал");
        EXPECT_DOUBLE_EQ(sampleSignal(InputKind::Step, 3, -2.0), -2.0);
        EXPECT_DOUBLE_EQ(sampleSignal(InputKind::Impulse, 0, -2.0), -2.0);
        EXPECT_NEAR(sampleSignal(InputKind::Harmonic, 2, -2.0), -2.0 * std::sin(2.0), 1e-12);
    }
    {

        SCOPED_TRACE("недопустимый тип возвращает ноль");
        const auto invalid = static_cast<InputKind>(99);
        EXPECT_DOUBLE_EQ(sampleSignal(invalid, 0, 5.0), 0.0);
        EXPECT_DOUBLE_EQ(sampleSignal(invalid, 7, -3.0), 0.0);
    }
}

TEST(InputSource, WrappersDelegateCorrectly)
{
    {
        SCOPED_TRACE("SignalInputSource делегирует в sampleSignal");
        const SignalInputSource step(InputKind::Step, 1.25);
        EXPECT_DOUBLE_EQ(step.valueAt(0), 1.25);
        EXPECT_DOUBLE_EQ(step.valueAt(7), 1.25);
        EXPECT_EQ(step.getType(), InputKind::Step);
        EXPECT_DOUBLE_EQ(step.getAmplitude(), 1.25);

        const SignalInputSource impulse(InputKind::Impulse, 4.0);
        EXPECT_DOUBLE_EQ(impulse.valueAt(0), 4.0);
        EXPECT_DOUBLE_EQ(impulse.valueAt(1), 0.0);
    }
    {
        SCOPED_TRACE("CallableInputSource использует переданную функцию");
        const CallableInputSource source([](int tau) { return static_cast<double>(tau) * 10.0; });
        EXPECT_DOUBLE_EQ(source.valueAt(0), 0.0);
        EXPECT_DOUBLE_EQ(source.valueAt(3), 30.0);
    }
    {
        SCOPED_TRACE("CallableInputSource с пустой функцией возвращает ноль");
        const CallableInputSource source(nullptr);
        EXPECT_DOUBLE_EQ(source.valueAt(0), 0.0);
        EXPECT_DOUBLE_EQ(source.valueAt(10), 0.0);
    }
}

class MockInputSource : public IInputSource
{
public:
    MOCK_METHOD(double, valueAt, (int tau), (const, override));
};

TEST(MockSignal, MockCanEmulateSignals)
{
    {
        SCOPED_TRACE("ступенька через ON_CALL");
        NiceMock<MockInputSource> input;
        ON_CALL(input, valueAt(::testing::_)).WillByDefault(Return(2.0));
        EXPECT_CALL(input, valueAt(::testing::_)).Times(::testing::AtLeast(3));

        EXPECT_DOUBLE_EQ(input.valueAt(0), 2.0);
        EXPECT_DOUBLE_EQ(input.valueAt(5), 2.0);
        EXPECT_DOUBLE_EQ(input.valueAt(99), 2.0);
    }
    {
        SCOPED_TRACE("импульс через EXPECT_CALL");
        MockInputSource input;
        EXPECT_CALL(input, valueAt(0)).WillOnce(Return(3.0));
        EXPECT_CALL(input, valueAt(::testing::Ne(0))).WillRepeatedly(Return(0.0));

        EXPECT_DOUBLE_EQ(input.valueAt(0), 3.0);
        EXPECT_DOUBLE_EQ(input.valueAt(1), 0.0);
        EXPECT_DOUBLE_EQ(input.valueAt(2), 0.0);
    }
}

TEST(MockSignal, SimulationUsesMockedInput)
{
    {
        SCOPED_TRACE("полная интеграция: модель 3.10 с мок-сигналом");

        Model3_10 model(0.0, 0.0);
        NiceMock<MockInputSource> input;
        ON_CALL(input, valueAt(::testing::_)).WillByDefault(Return(2.0));

        const auto rows = simulate(model, SimulationConfig{3, 0.0, 0.5}, input);
        ASSERT_EQ(rows.size(), 4u);
        EXPECT_NEAR(rows[0].y, 0.0, 1e-12);
        EXPECT_NEAR(rows[1].y, 1.0, 1e-12);
        EXPECT_NEAR(rows[2].y, 2.0, 1e-12);
        EXPECT_NEAR(rows[3].y, 3.0, 1e-12);
    }
    {
        SCOPED_TRACE("движок запрашивает вход на каждом такте");
        NiceMock<MockInputSource> input;
        ON_CALL(input, valueAt(::testing::_)).WillByDefault(Return(0.0));
        EXPECT_CALL(input, valueAt(0)).Times(::testing::AtLeast(1));
        EXPECT_CALL(input, valueAt(1)).Times(::testing::AtLeast(1));
        EXPECT_CALL(input, valueAt(2)).Times(::testing::AtLeast(1));

        Model3_10 model(0.5, 1.0);
        simulate(model, SimulationConfig{2, 0.0, 0.1}, input);
    }
}
