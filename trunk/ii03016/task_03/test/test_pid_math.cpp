#include <gtest/gtest.h>
#include "../src/PID.h"
#include <cmath>

// 8. Комбинация Kp + Ki
TEST(PIDMath, KpPlusKi) {
    PID pid(1.0, 1.0, 0.0, 0.1);
    EXPECT_DOUBLE_EQ(pid.compute(10.0, 0.0), 11.0);
}

// 9. Комбинация Kp + Kd
TEST(PIDMath, KpPlusKd) {
    PID pid(1.0, 0.0, 0.1, 0.1);
    EXPECT_DOUBLE_EQ(pid.compute(10.0, 0.0), 20.0);
}

// 10. Все три коэффициента
TEST(PIDMath, AllCoefficients) {
    PID pid(1.0, 0.1, 0.01, 0.1);
    EXPECT_NEAR(pid.compute(10.0, 0.0), 11.1, 1e-9);
}

// 11. Ограничение сверху (max_u)
TEST(PIDMath, MaxLimit) {
    PID pid(100.0, 0.0, 0.0, 0.1, -10.0, 10.0);
    EXPECT_DOUBLE_EQ(pid.compute(100.0, 0.0), 10.0);
}

// 12. Ограничение снизу (min_u)
TEST(PIDMath, MinLimit) {
    PID pid(100.0, 0.0, 0.0, 0.1, -10.0, 10.0);
    EXPECT_DOUBLE_EQ(pid.compute(-100.0, 0.0), -10.0);
}