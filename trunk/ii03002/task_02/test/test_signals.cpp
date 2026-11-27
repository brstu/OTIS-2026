#include <gtest/gtest.h>
#include <cmath>
#include "InputSignal.h"

TEST(InputSignal, StepSignalIsConstant) {
    for (int k = 0; k < 8; ++k) EXPECT_DOUBLE_EQ(InputSignal::getSignal(SignalType::STEP, k), 1.0);
}
TEST(InputSignal, ImpulseIsNonzeroOnlyAtFirstStep) {
    EXPECT_DOUBLE_EQ(InputSignal::getSignal(SignalType::IMPULSE, 0), 1.0);
    for (int k = 1; k < 8; ++k) EXPECT_DOUBLE_EQ(InputSignal::getSignal(SignalType::IMPULSE, k), 0.0);
}
TEST(InputSignal, HarmonicSignalMatchesImplementedFrequency) {
    EXPECT_DOUBLE_EQ(InputSignal::getSignal(SignalType::HARMONIC, 0), 0.0);
    EXPECT_NEAR(InputSignal::getSignal(SignalType::HARMONIC, 1), std::sin(0.4), 1e-12);
    EXPECT_NEAR(InputSignal::getSignal(SignalType::HARMONIC, 3), std::sin(1.2), 1e-12);
}
TEST(InputSignal, SignalNamesAreCorrect) {
    EXPECT_EQ(InputSignal::getSignalName(SignalType::STEP), "Step");
    EXPECT_EQ(InputSignal::getSignalName(SignalType::IMPULSE), "Impulse");
    EXPECT_EQ(InputSignal::getSignalName(SignalType::HARMONIC), "Harmonic");
    EXPECT_EQ(InputSignal::getSignalName(static_cast<SignalType>(99)), "Unknown");
}
