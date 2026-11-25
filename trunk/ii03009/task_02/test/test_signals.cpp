#include <gtest/gtest.h>
#include "../src/InputSignal.h"

TEST(InputSignalTest, StepResponse) {
    EXPECT_EQ(InputSignal::step(-1), 0.0);
    EXPECT_EQ(InputSignal::step(0), 1.0);
}

TEST(InputSignalTest, ImpulseResponse) {
    EXPECT_EQ(InputSignal::impulse(0), 1.0);
}