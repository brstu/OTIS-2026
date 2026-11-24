#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <cmath>
#include <vector>
#include "InputSignal.h"
#include "Model1_6.h"

class ISignalProvider {
public:
    virtual ~ISignalProvider() = default;
    virtual double sample(int tau) const = 0;
};

class MockSignalProvider : public ISignalProvider {
public:
    MOCK_METHOD(double, sample, (int tau), (const, override));
};

TEST(SignalAndMockTest, StepImpulseAndHarmonicSignalsMatchInputSignal) {
    EXPECT_DOUBLE_EQ(InputSignal::getSignal(SignalType::STEP, 0), 1.0);
    EXPECT_DOUBLE_EQ(InputSignal::getSignal(SignalType::STEP, 5), 1.0);

    EXPECT_DOUBLE_EQ(InputSignal::getSignal(SignalType::IMPULSE, 0), 1.0);
    EXPECT_DOUBLE_EQ(InputSignal::getSignal(SignalType::IMPULSE, 1), 0.0);

    EXPECT_NEAR(InputSignal::getSignal(SignalType::HARMONIC, 0), 0.0, 1e-12);
    EXPECT_NEAR(InputSignal::getSignal(SignalType::HARMONIC, 2), std::sin(2.0), 1e-12);
}

TEST(SignalAndMockTest, SimulationWithMockedInputSignal) {
    MockSignalProvider mock_signal;
    EXPECT_CALL(mock_signal, sample(0)).WillOnce(::testing::Return(1.0));
    EXPECT_CALL(mock_signal, sample(1)).WillOnce(::testing::Return(0.0));
    EXPECT_CALL(mock_signal, sample(2)).WillOnce(::testing::Return(1.0));

    Model1_6 model(0.5, 0.0, 0.0, 1.0, 0.0);
    std::vector<double> outputs;
    for (int t = 0; t < 3; ++t) {
        const double u = mock_signal.sample(t);
        outputs.push_back(model.step(u));
    }

    EXPECT_NEAR(outputs[0], 1.0, 1e-9);
    EXPECT_NEAR(outputs[1], 0.5, 1e-9);
    EXPECT_NEAR(outputs[2], 1.25, 1e-9);
}