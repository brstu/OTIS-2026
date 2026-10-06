#pragma once

#include "DynamicProcess.h"

// =====================================================================
// QuadraticFeedback — Model 2.1
// (Quadratic Feedback and Harmonic Control)
//
// Формула:
//     y(t+1) = a*y(t) - b*y(t-1)^2 + c*u(t) + d*sin(u(t-1))
//
// Нелинейная модель: содержит квадрат предыдущего выхода
// и синусоидальную составляющую управления.
// =====================================================================
class QuadraticFeedback : public DynamicProcess
{
private:
    // Коэффициенты модели.
    double gain_linear;     // a — линейная обратная связь
    double gain_quadratic;  // b — квадратичная обратная связь
    double gain_input;      // c — коэффициент текущего входа
    double gain_harmonic;   // d — амплитуда гармонического воздействия

    // Состояние модели.
    double y_quad = 0.0;      // текущее значение выхода
    double y_quad_prev = 0.0; // y(t-1)
    double u_quad_prev = 0.0; // u(t-1)

public:
    // Конструктор.
    QuadraticFeedback(double a_, double b_, double c_, double d_);

    // Один шаг симуляции.
    double advance(double u) override;

    // Сброс состояния.
    void reset() override;

    // Название модели.
    std::string title() const override;
};
