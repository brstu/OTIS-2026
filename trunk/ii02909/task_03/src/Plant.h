#pragma once

/**
 * @file Plant.h
 * @brief Объект управления (Plant) на основе модели 1.4 из ЛР №1.
 * 
 * Модель: Дискретный интегратор с потерями.
 * Формула: y_{k+1} = a * y_k + b * (u_k - u_{k-1})
 */

class Plant {
private:
    double a;       ///< Коэффициент затухания (потери)
    double b;       ///< Коэффициент усиления по входу
    double y;       ///< Текущее состояние (выходная переменная)
    double u_prev;  ///< Предыдущее управляющее воздействие

public:
    /**
     * @brief Конструктор объекта управления.
     * @param a_param Коэффициент a
     * @param b_param Коэффициент b
     */
    Plant(double a_param, double b_param) : a(a_param), b(b_param), y(0.0), u_prev(0.0) {}

    /**
     * @brief Сброс состояния объекта в начальное (y=0, u_prev=0).
     */
    void reset() {
        y = 0.0;
        u_prev = 0.0;
    }

    /**
     * @brief Выполнить один шаг симуляции объекта.
     * @param u Текущее управляющее воздействие u_k
     * @return Новое состояние выхода y_{k+1}
     */
    double step(double u) {
        double y_next = a * y + b * (u - u_prev);
        u_prev = u;
        y = y_next;
        return y;
    }

    /**
     * @brief Получить текущее значение выхода.
     * @return Текущее y
     */
    double getOutput() const { return y; }
};