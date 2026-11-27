// Tests of the input signals: step, impulse, harmonic.
#include <cmath>

#include <gtest/gtest.h>

#include "signals.h"

namespace {
constexpr double kPi = 3.14159265358979323846;
}

// ---------- step ----------
TEST(StepInput, ReturnsTheSameValueOnEveryStep) {
    const StepInput s(2.5);
    for (int tau = 0; tau < 10; ++tau) {
        EXPECT_DOUBLE_EQ(s.value(tau, tau * 0.1), 2.5);
    }
}

TEST(StepInput, NameAndTag) {
    const StepInput s(2.5);
    EXPECT_EQ(s.tag(), "step");
    EXPECT_EQ(s.name(), "step (u = 2.5)");
}

class StepValueTest : public ::testing::TestWithParam<double> {};

TEST_P(StepValueTest, ValueDoesNotDependOnStepNumberOrTime) {
    const double c = GetParam();
    const StepInput s(c);
    EXPECT_DOUBLE_EQ(s.value(0, 0.0), c);
    EXPECT_DOUBLE_EQ(s.value(7, 0.7), c);
    EXPECT_DOUBLE_EQ(s.value(1000, 1e6), c);
}

INSTANTIATE_TEST_SUITE_P(Amplitudes, StepValueTest, ::testing::Values(-3.5, 0.0, 1.0, 42.0));

// ---------- impulse ----------
TEST(ImpulseInput, IsOneOnlyOnTheFirstStep) {
    const ImpulseInput s;
    EXPECT_DOUBLE_EQ(s.value(0, 0.0), 1.0);
    EXPECT_DOUBLE_EQ(s.value(1, 1.0), 0.0);
    EXPECT_DOUBLE_EQ(s.value(2, 2.0), 0.0);
    EXPECT_DOUBLE_EQ(s.value(100, 100.0), 0.0);
}

TEST(ImpulseInput, NameAndTag) {
    const ImpulseInput s;
    EXPECT_EQ(s.tag(), "impulse");
    EXPECT_FALSE(s.name().empty());
}

// ---------- harmonic ----------
TEST(HarmonicInput, EqualsSinOfTime) {
    const HarmonicInput s;
    EXPECT_NEAR(s.value(0, 0.0), 0.0, 1e-12);
    EXPECT_NEAR(s.value(1, kPi / 2.0), 1.0, 1e-12);
    EXPECT_NEAR(s.value(2, kPi), 0.0, 1e-12);
    EXPECT_NEAR(s.value(3, 3.0 * kPi / 2.0), -1.0, 1e-12);
}

TEST(HarmonicInput, UsesTimeNotStepNumber) {
    // For the ODE model t = tau*dt, so the step number alone must not change the value.
    const HarmonicInput s;
    EXPECT_DOUBLE_EQ(s.value(5, 0.0), 0.0);
    EXPECT_DOUBLE_EQ(s.value(0, 1.0), std::sin(1.0));
}

TEST(HarmonicInput, NameAndTag) {
    const HarmonicInput s;
    EXPECT_EQ(s.tag(), "harmonic");
    EXPECT_FALSE(s.name().empty());
}

// ---------- helper num() ----------
TEST(Num, ConvertsNumbersToShortText) {
    EXPECT_EQ(num(0.5), "0.5");
    EXPECT_EQ(num(3.0), "3");
    EXPECT_EQ(num(-1.25), "-1.25");
}
