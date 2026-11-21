#include <gtest/gtest.h>
#include <cmath>
#include "InputGenerator.h"

TEST(InputGenerator, ConstantLabel)
{
    ConstantInput signal(1.0);
    EXPECT_EQ(signal.getLabel(), "Constant (u = 1.000000)");
}

TEST(InputGenerator, ConstantValue)
{
    ConstantInput signal(1.0);
    EXPECT_NEAR(signal.getValue(1), 1.0, 1e-9);
    EXPECT_NEAR(signal.getValue(10), 1.0, 1e-9);
}

TEST(InputGenerator, PulseLabel)
{
    PulseInput signal;
    EXPECT_EQ(signal.getLabel(), "Pulse (u_1 = 1, u_tau>1 = 0)");
}

TEST(InputGenerator, PulseValue)
{
    PulseInput signal;
    EXPECT_NEAR(signal.getValue(1), 1.0, 1e-9);
    EXPECT_NEAR(signal.getValue(2), 0.0, 1e-9);
    EXPECT_NEAR(signal.getValue(10), 0.0, 1e-9);
}

TEST(InputGenerator, SineLabel)
{
    SineInput signal;
    EXPECT_EQ(signal.getLabel(), "Sine (u_tau = sin(tau))");
}

TEST(InputGenerator, SineValue)
{
    SineInput signal;
    EXPECT_NEAR(signal.getValue(1), std::sin(1.0), 1e-9);
    EXPECT_NEAR(signal.getValue(2), std::sin(2.0), 1e-9);
}