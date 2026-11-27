#pragma once

#include <fstream>
#include <string>

/**
 * @file Simulation.h
 * @brief Заголовочный файл для модуля симуляции системы управления.
 */

/**
 * @brief Перечисление доступных моделей объектов управления (из ЛР1).
 */
enum class PlantType {
    Linear_1_10,     ///< Линейная модель 1.10
    Nonlinear_2_5,   ///< Нелинейная модель 2.5
    Diff_3_10        ///< Дифференциальное уравнение 3.10
};

/**
 * @brief Класс, управляющий процессом симуляции.
 */
class Simulation {
private:
    PlantType current_plant; ///< Текущий выбранный объект управления
    double dt;               ///< Шаг интегрирования (для дифференциальных моделей)

public:
    /**
     * @brief Конструктор симуляции.
     * @param plant_type Тип объекта управления.
     * @param dt_ Шаг времени.
     */
    explicit Simulation(PlantType plant_type, double dt_ = 0.1);

    /**
     * @brief Запуск симуляции в режиме А (фиксированное число шагов).
     * @param setpoint Заданное значение (уставка).
     * @param n_steps Количество шагов.
     * @param Kp Пропорциональный коэффициент.
     * @param Ki Интегральный коэффициент.
     * @param Kd Дифференциальный коэффициент.
     */
    void runFixedSteps(double setpoint, int n_steps, double Kp, double Ki, double Kd);

    /**
     * @brief Запуск симуляции в режиме Б (до достижения установившегося режима).
     * @param setpoint Заданное значение (уставка).
     * @param epsilon Допустимая ошибка (|e| < epsilon).
     * @param max_steps Максимальное число шагов (защита от зацикливания).
     * @param Kp Пропорциональный коэффициент.
     * @param Ki Интегральный коэффициент.
     * @param Kd Дифференциальный коэффициент.
     */
    void runUntilSteadyState(double setpoint, double epsilon, int max_steps, double Kp, double Ki, double Kd);
};