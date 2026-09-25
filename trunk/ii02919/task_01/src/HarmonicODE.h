#pragma once

#include "DynamicProcess.h"

// ============================================================================
//  HarmonicODE — Model 3.5
//  Harmonic Driving Force
//
//  Дифференциальное уравнение:
//    dy/dt = b * sin(u)
//
//  Решается методом Эйлера:
//    y(t+1) = y(t) + dt * b * sin(u(t))
//
//  Начальное условие y(0) = 0 (для интегрирования от нуля).
// ============================================================================
class HarmonicODE : public DynamicProcess
{
private:
    double b;      // амплитуда управляющего воздействия
    double dt;     // шаг интегрирования
    double y;      // текущее значение выхода

public:
    explicit HarmonicODE(double b_, double dt_ = 0.01);

    double advance(double u) override;
    void reset() override;
    std::string title() const override;

    // Устойчивость численной схемы Эйлера всегда выполняется,
    // так как коэффициент при y в правой части равен 0.
};
