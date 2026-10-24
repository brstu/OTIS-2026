#pragma once

/**
 * @file PIDController.h
 * @brief Реализация дискретного ПИД-регулятора (инкрементная форма).
 * 
 * Используется для управления объектом (Plant) с целью минимизации ошибки.
 */

class PIDController {
private:
    double Kp, Ki, Kd; ///< Коэффициенты ПИД
    double T0;         ///< Шаг квантования (dt)
    
    double e_prev1;    ///< Предыдущая ошибка e(k-1)
    double integral;   ///< Накопленный интеграл
    double u_prev;     ///< Предыдущее управление

    double u_min, u_max; ///< Ограничения управления (Anti-windup)

public:
    /**
     * @brief Конструктор регулятора.
     * @param kp Пропорциональный коэффициент (Kp)
     * @param ki Интегральный коэффициент (Ki)
     * @param kd Дифференциальный коэффициент (Kd)
     * @param dt Шаг квантования (T0)
     * @param minU Минимальное значение управления
     * @param maxU Максимальное значение управления
     */
    PIDController(double kp, double ki, double kd, double dt, double minU = -1e6, double maxU = 1e6);

    /**
     * @brief Сброс состояния регулятора в начальное.
     */
    void reset();

    /**
     * @brief Вычислить следующее управляющее воздействие.
     * @param error Текущая ошибка e(k) = w(k) - y(k)
     * @return Управляющее воздействие u(k)
     */
    double compute(double error);
};