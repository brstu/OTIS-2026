#include <gtest/gtest.h>
#include "../src/PID.h"

// 1. Пропорциональная составляющая
TEST(PIDBasic, Proportional) {
    PID pid(2.0, 0.0, 0.0, 0.1);
    EXPECT_DOUBLE_EQ(pid.compute(10.0, 5.0), 10.0);
}

// 2. Интегральная составляющая
TEST(PIDBasic, Integral) {
    PID pid(0.0, 1.0, 0.0, 0.1);
    EXPECT_DOUBLE_EQ(pid.compute(10.0, 0.0), 1.0);
    EXPECT_DOUBLE_EQ(pid.compute(10.0, 0.0), 2.0);
}

// 3. Дифференциальная составляющая
TEST(PIDBasic, Derivative) {
    PID pid(0.0, 0.0, 2.0, 0.1);
    EXPECT_DOUBLE_EQ(pid.compute(10.0, 0.0), 100.0); // Ограничено max_u
}

// 4. Anti-windup
TEST(PIDBasic, AntiWindup) {
    PID pid(0.0, 1.0, 0.0, 0.1, -5.0, 5.0);
    for (int i = 0; i < 100; ++i) pid.compute(100.0, 0.0);
    EXPECT_LE(pid.compute(100.0, 0.0), 5.0);
}

// 5. Сброс состояния
TEST(PIDBasic, Reset) {
    PID pid(1.0, 1.0, 1.0, 0.1);
    pid.compute(10.0, 0.0);
    pid.reset();
    EXPECT_DOUBLE_EQ(pid.compute(0.0, 0.0), 0.0);
}

// 6. Нулевая ошибка
TEST(PIDBasic, ZeroError) {
    PID pid(1.0, 1.0, 1.0, 0.1);
    EXPECT_DOUBLE_EQ(pid.compute(5.0, 5.0), 0.0);
}

// 7. Отрицательная ошибка
TEST(PIDBasic, NegativeError) {
    PID pid(1.0, 0.0, 0.0, 0.1);
    EXPECT_DOUBLE_EQ(pid.compute(5.0, 10.0), -5.0);
}