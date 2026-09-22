#pragma once

#include "DynamicProcess.h"

// ============================================================================
//  QuadraticFeedback — Model 2.1
//  Quadratic Feedback and Harmonic Control
//
//  Формула:
//    y(t+1) = a*y(t) - b*y(t-1)^2 + c*u(t) + d*sin(u(t-1))
//
//  Нелинейная модель: содержит квадрат предыдущего выхода
//  и синусоидальную составляющую управления.
// ============================================================================
class QuadraticFeedback : public DynamicProcess
{
private:
    double a;    // коэффициент линейной обратной связи
    double b;    // коэффициент квадратичной обратной связи
    double c;    // коэффициент текущего входа
    double d;    // амплитуда гармонического воздействия

    double y;         // текущее значение выхода
    double y_prev;    // y(t-1)
    double u_prev;    // u(t-1)

public:
    QuadraticFeedback(double a_, double b_, double c_, double d_);

    double advance(double u) override;
    void reset() override;
    std::string title() const override;

    // Нелинейная модель — анализ устойчивости неприменим,
    // методы isStableSystem() и alertText() наследуются от базового класса.
};

