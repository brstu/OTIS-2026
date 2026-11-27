#include <gtest/gtest.h>
#include "../src/PID.h"
#include <cmath>

// 13. Постепенное изменение ошибки
TEST(PIDDynamic, DecreasingError) {
    PID pid(1.0, 0.1, 0.0, 0.1);
    double y = 0.0;
    for (int i = 0; i < 50; ++i) {
        double u = pid.compute(100.0, y);
        y += u * 0.1;
    }
    EXPECT_NEAR(y, 100.0, 10.0);
}

// 14. Изменение уставки
TEST(PIDDynamic, ChangeSetpoint) {
    PID pid(1.0, 0.0, 0.0, 0.1);
    EXPECT_DOUBLE_EQ(pid.compute(10.0, 0.0), 10.0);
    EXPECT_DOUBLE_EQ(pid.compute(20.0, 0.0), 20.0);
}

// 15. Импульсное воздействие
TEST(PIDDynamic, Impulse) {
    PID pid(1.0, 0.0, 0.0, 0.1);
    EXPECT_DOUBLE_EQ(pid.compute(10.0, 0.0), 10.0);
    EXPECT_DOUBLE_EQ(pid.compute(0.0, 0.0), 0.0);
}

// 16. Плавное изменение уставки (линейный рост)
TEST(PIDDynamic, RampSetpoint) {
    PID pid(1.0, 0.0, 0.0, 0.1);
    double y = 0.0;
    for (int i = 0; i < 10; ++i) {
        double setpoint = i * 10.0;
        double u = pid.compute(setpoint, y);
        y += u * 0.1;
    }
    EXPECT_GT(y, 0.0);
}

// 17. Синусоидальная уставка
TEST(PIDDynamic, SinusoidalSetpoint) {
    PID pid(1.0, 0.0, 0.0, 0.1);
    double y = 0.0;
    for (int i = 0; i < 20; ++i) {
        double setpoint = 50.0 + 10.0 * std::sin(i * 0.1);
        double u = pid.compute(setpoint, y);
        y += u * 0.1;
    }
    EXPECT_GT(y, 0.0);
}

// 18. Проверка на устойчивость при больших Kp
TEST(PIDDynamic, HighKp) {
    PID pid(100.0, 0.0, 0.0, 0.1, -1000.0, 1000.0);
    double u = pid.compute(10.0, 0.0);
    EXPECT_GT(u, 0.0);
}

// 19. Проверка на устойчивость при малых Kp
TEST(PIDDynamic, LowKp) {
    PID pid(0.01, 0.0, 0.0, 0.1);
    double u = pid.compute(10.0, 0.0);
    EXPECT_NEAR(u, 0.1, 1e-9);
}

// 20. Проверка Kd при резком изменении
TEST(PIDDynamic, KdSpike) {
    PID pid(0.0, 0.0, 10.0, 0.1, -1000.0, 1000.0);
    double u = pid.compute(10.0, 0.0);
    EXPECT_NEAR(u, 1000.0, 1e-9); // Ограничено
}

// 21. Модель объекта с инерцией
TEST(PIDDynamic, InertialPlant) {
    PID pid(2.0, 0.5, 0.1, 0.1);
    double y = 0.0;
    for (int i = 0; i < 100; ++i) {
        double u = pid.compute(50.0, y);
        y += 0.05 * (u - y); // Инерционная модель
    }
    EXPECT_NEAR(y, 50.0, 5.0);
}

// 22. Проверка при нулевых коэффициентах
TEST(PIDDynamic, ZeroCoefficients) {
    PID pid(0.0, 0.0, 0.0, 0.1);
    EXPECT_DOUBLE_EQ(pid.compute(100.0, 0.0), 0.0);
}

// 23. Проверка при отрицательных коэффициентах
TEST(PIDDynamic, NegativeCoefficients) {
    PID pid(-1.0, 0.0, 0.0, 0.1);
    EXPECT_DOUBLE_EQ(pid.compute(10.0, 0.0), -10.0);
}

// 24. Проверка сброса после насыщения
TEST(PIDDynamic, ResetAfterSaturation) {
    PID pid(1.0, 1.0, 0.0, 0.1, -5.0, 5.0);
    for (int i = 0; i < 100; ++i) pid.compute(100.0, 0.0);
    pid.reset();
    EXPECT_DOUBLE_EQ(pid.compute(0.0, 0.0), 0.0);
}

// 25. Проверка стабильности при больших Kd
TEST(PIDDynamic, HighKd) {
    PID pid(0.0, 0.0, 100.0, 0.1, -1000.0, 1000.0);
    double u = pid.compute(10.0, 0.0);
    EXPECT_NEAR(u, 1000.0, 1e-9);
}