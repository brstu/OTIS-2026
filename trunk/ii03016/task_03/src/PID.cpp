#include "PID.h"
#include <algorithm> // Для std::clamp (ограничение)

/**
 * @brief Конструктор ПИД-регулятора.
 */
PID::PID::PID(double Kp_, double Ki_, double Kd_, double dt_, double min_u_, double max_u_)
    : Kp(Kp_), Ki(Ki_), Kd(Kd_), dt(dt_), min_u(min_u_), max_u(max_u_) {}


/**
 * @brief Сброс состояния регулятора.
 */
void PID::reset() {
    prev_error = 0.0;
    prev_error2 = 0.0;
    integral = 0.0;
}

/**
 * @brief Вычисление управляющего воздействия.
 * @details Реализован позиционный алгоритм с защитой от насыщения.
 */
double PID::compute(double setpoint, double current_value) {
    // 1. Вычисляем текущую ошибку
    double error = setpoint - current_value;

    // 2. Интегральная часть (накапливаем сумму)
    integral += error * dt;

    // 3. Дифференциальная часть (разность)
    double derivative = (error - prev_error) / dt;

    // 4. Вычисляем управляющее воздействие
    double u = Kp * error + Ki * integral + Kd * derivative;

    // 5. Anti-windup (защита от интегрального насыщения)
    if (u > max_u) {
        u = max_u;
        integral -= error * dt; // Откатываем интеграл назад, если уперлись в предел
    } else if (u < min_u) {
        u = min_u;
        integral -= error * dt; // Откатываем интеграл назад
    }

    // 6. Сохраняем ошибку для следующего шага
    prev_error2 = prev_error;
    prev_error = error;

    return u;
}