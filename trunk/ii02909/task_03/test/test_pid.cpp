#include <gtest/gtest.h>
#include "PIDController.h"

TEST(PIDTest, ZeroErrorKeepsControlZero) {
    PIDController pid(1.0, 0.1, 0.05, 0.01);
    pid.reset();
    // Если ошибка равна нулю, управление не должно меняться (остается 0)
    EXPECT_NEAR(pid.compute(0.0), 0.0, 1e-6);
    EXPECT_NEAR(pid.compute(0.0), 0.0, 1e-6);
}

TEST(PIDTest, AntiWindupLimits) {
    // Ограничиваем управление от -5 до 5
    PIDController pid(10.0, 0.0, 0.0, 0.01, -5.0, 5.0);
    pid.reset();
    // Подаем огромную ошибку, управление должно упереться в предел
    double u = pid.compute(1000.0);
    EXPECT_NEAR(u, 5.0, 1e-6); 
}