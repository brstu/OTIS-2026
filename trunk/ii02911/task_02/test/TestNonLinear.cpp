#include <gtest/gtest.h>
#include <cmath>
#include "InputSignal.h"

TEST(InputSignalTest, StepSignalGeneration)
{
    EXPECT_DOUBLE_EQ(getInput(SignalType::Step, 0, 1.0), 1.0);
    EXPECT_DOUBLE_EQ(getInput(SignalType::Step, 5, 1.0), 1.0);
    EXPECT_DOUBLE_EQ(getInput(SignalType::Step, 10, 2.0), 2.0);
}

TEST(InputSignalTest, ImpulseSignalGeneration)
{
    EXPECT_DOUBLE_EQ(getInput(SignalType::Impulse, 0, 1.0), 1.0);
    EXPECT_DOUBLE_EQ(getInput(SignalType::Impulse, 1, 1.0), 0.0);
    EXPECT_DOUBLE_EQ(getInput(SignalType::Impulse, 5, 2.0), 0.0);
}

TEST(InputSignalTest, HarmonicSignalGeneration)
{
    EXPECT_NEAR(getInput(SignalType::Harmonic, 0, 1.0), 0.0, 1e-5);
    EXPECT_NEAR(getInput(SignalType::Harmonic, 1, 1.0), sin(1.0), 1e-5);
}