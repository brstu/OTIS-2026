#pragma once

/**
 * @file PID.h
 * @brief Заголовочный файл для дискретного ПИД-регулятора.
 * @details Реализует инкрементную форму ПИД-регулятора с защитой от интегрального насыщения (Anti-windup).
 */

/**
 * @class PID
 * @brief Класс, реализующий дискретный ПИД-регулятор.
 */
class PID {
private:
    double Kp;
    double Ki;
    double Kd;
    double dt;
    double min_u;
    double max_u;
    double prev_error = 0.0;
    double prev_error2 = 0.0;
    double integral = 0.0;

public:
    /**
     * @brief Конструктор ПИД-регулятора.
     * @param Kp_ Пропорциональный коэффициент.
     * @param Ki_ Интегральный коэффициент.
     * @param Kd_ Дифференциальный коэффициент.
     * @param dt_ Шаг квантования.
     * @param min_u_ Минимальное значение управляющего сигнала.
     * @param max_u_ Максимальное значение управляющего сигнала.
     */
    PID(double Kp_, double Ki_, double Kd_, double dt_, double min_u_ = -100.0, double max_u_ = 100.0);

    /**
     * @brief Сброс состояния регулятора (обнуление интеграла и ошибок).
     */
    void reset();

    /**
     * @brief Вычисление управляющего воздействия.
     * @param setpoint Заданное значение (уставка).
     * @param current_value Текущее значение объекта управления.
     * @return double Управляющее воздействие u(k).
     */
    double compute(double setpoint, double current_value);
};