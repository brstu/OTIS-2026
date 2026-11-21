#include "PIDController.h"
#include <algorithm>

PIDController::PIDController(double kp, double ki, double kd, double dt, double minU, double maxU)
    : Kp(kp), Ki(ki), Kd(kd), T0(dt), u_min(minU), u_max(maxU) 
{
    reset();
}

void PIDController::reset() {
    e_prev1 = 0.0;
    integral = 0.0;
    u_prev = 0.0;
}

double PIDController::compute(double error) {
    // 1. Пропорциональная часть
    double P = Kp * error;

    // 2. Интегральная часть (накопление площади)
    integral += error * T0;
    double I = Ki * integral;

    // 3. Дифференциальная часть (скорость изменения)
    double derivative = (error - e_prev1) / T0;
    double D = Kd * derivative;

    // 4. Итоговое управление
    double u = P + I + D;

    // 5. Ограничение (Anti-windup)
    if (u > u_max) {
        u = u_max;
        integral -= error * T0; // Откатываем интеграл
    }
    if (u < u_min) {
        u = u_min;
        integral -= error * T0;
    }

    // Обновление памяти
    e_prev1 = error;
    u_prev = u;

    return u;
}