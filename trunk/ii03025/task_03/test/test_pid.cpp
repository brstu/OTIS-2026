#include <gtest/gtest.h>
#include "pid.h"

// Проверка пропорциональной составляющей
TEST(PIDTest, ProportionalControl) {
    PIDController pid(2.0, 0.0, 0.0, -100.0, 100.0);

    double result = pid.calculate(10.0, 0.0, 1.0);

    EXPECT_DOUBLE_EQ(result, 20.0);
}

// Проверка ограничения управляющего сигнала
TEST(PIDTest, ControlLimit) {
    PIDController pid(10.0, 0.0, 0.0, -5.0, 5.0);

    double result = pid.calculate(10.0, 0.0, 1.0);

    EXPECT_DOUBLE_EQ(result, 5.0);
}

// Проверка интегральной составляющей
TEST(PIDTest, IntegralControl) {
    PIDController pid(0.0, 2.0, 0.0, -100.0, 100.0);

    double result = pid.calculate(5.0, 0.0, 1.0);

    EXPECT_DOUBLE_EQ(result, 10.0);
}

// Проверка дифференциальной составляющей
TEST(PIDTest, DifferentialControl) {
    PIDController pid(0.0, 0.0, 2.0, -100.0, 100.0);

    double result = pid.calculate(5.0, 0.0, 1.0);

    EXPECT_DOUBLE_EQ(result, 10.0);
}

// Проверка сброса регулятора
TEST(PIDTest, ResetController) {
    PIDController pid(2.0, 0.0, 0.0, -100.0, 100.0);

    pid.calculate(10.0, 0.0, 1.0);
    pid.reset();

    double result = pid.calculate(0.0, 0.0, 1.0);

    EXPECT_DOUBLE_EQ(result, 0.0);
}

// Проверка нулевого шага времени
TEST(PIDTest, ZeroTimeStep) {
    PIDController pid(2.0, 1.0, 1.0, -100.0, 100.0);

    double result = pid.calculate(10.0, 0.0, 0.0);

    EXPECT_DOUBLE_EQ(result, 0.0);
}
