#include <gtest/gtest.h>

#include <cmath>

#include "InputSignals.h"

namespace {
const double kPi = 3.14159265358979323846;
}

// ---------------------------------------------------------- ступенчатый

TEST(InputSignals, ConstantReturnsAmplitudeAtAnyStep) {
    EXPECT_DOUBLE_EQ(generateInput(InputSignalType::Constant, 0, 0.0, 2.5, 1.0), 2.5);
    EXPECT_DOUBLE_EQ(generateInput(InputSignalType::Constant, 7, 0.7, 2.5, 1.0), 2.5);
    EXPECT_DOUBLE_EQ(generateInput(InputSignalType::Constant, 1000, 100.0, -4.0, 1.0), -4.0);
}

// ---------------------------------------------------------- импульсный

TEST(InputSignals, ImpulseIsNonZeroOnlyAtStepZero) {
    EXPECT_DOUBLE_EQ(generateInput(InputSignalType::Impulse, 0, 0.0, 3.0, 1.0), 3.0);
    EXPECT_DOUBLE_EQ(generateInput(InputSignalType::Impulse, 1, 0.1, 3.0, 1.0), 0.0);
    EXPECT_DOUBLE_EQ(generateInput(InputSignalType::Impulse, 50, 5.0, 3.0, 1.0), 0.0);
}

// ---------------------------------------------------------- гармонический

TEST(InputSignals, HarmonicIsZeroAtTimeZero) {
    EXPECT_DOUBLE_EQ(generateInput(InputSignalType::Harmonic, 0, 0.0, 2.0, 5.0), 0.0);
}

TEST(InputSignals, HarmonicReachesAmplitudeAtQuarterPeriod) {
    // omega = 1, t = pi/2 -> sin = 1
    EXPECT_NEAR(generateInput(InputSignalType::Harmonic, 0, kPi / 2.0, 2.0, 1.0), 2.0, 1e-12);
}

TEST(InputSignals, HarmonicReturnsToZeroAtHalfPeriod) {
    EXPECT_NEAR(generateInput(InputSignalType::Harmonic, 0, kPi, 2.0, 1.0), 0.0, 1e-12);
}

TEST(InputSignals, HarmonicIsOddFunctionOfTime) {
    const double positive = generateInput(InputSignalType::Harmonic, 0, 0.7, 1.5, 2.0);
    const double negative = generateInput(InputSignalType::Harmonic, 0, -0.7, 1.5, 2.0);

    EXPECT_NEAR(positive, -negative, 1e-12);
}

TEST(InputSignals, HarmonicIsPeriodic) {
    const double omega = 3.0;
    const double period = 2.0 * kPi / omega;

    EXPECT_NEAR(generateInput(InputSignalType::Harmonic, 0, 0.4, 1.0, omega),
                generateInput(InputSignalType::Harmonic, 0, 0.4 + period, 1.0, omega),
                1e-12);
}

// ---------------------------------------------------------- значения по умолчанию

TEST(InputSignals, DefaultAmplitudeAndOmegaAreOne) {
    EXPECT_DOUBLE_EQ(generateInput(InputSignalType::Constant, 0, 0.0), 1.0);
    EXPECT_NEAR(generateInput(InputSignalType::Harmonic, 0, kPi / 2.0), 1.0, 1e-12);
}

// ---------------------------------------------------------- защитная ветка

TEST(InputSignals, UnknownSignalTypeReturnsZero) {
    const auto unknown = static_cast<InputSignalType>(99);

    EXPECT_DOUBLE_EQ(generateInput(unknown, 3, 0.3, 5.0, 1.0), 0.0);
}
