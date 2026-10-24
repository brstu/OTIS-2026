#pragma once

#include "DynamicProcess.h"

// =====================================================================
// HarmonicODE — Model 3.5 (Harmonic Driving Force)
//
// Дифференциальное уравнение:
//     dy/dt = b * sin(u)
//
// Решается методом Эйлера:
//     y(t+1) = y(t) + dt * b * sin(u(t))
//
// Начальное условие: y(0) = 0.
// =====================================================================
class HarmonicODE : public DynamicProcess
{
private:
    // Амплитуда управляющего воздействия (коэффициент b).
    double amplitude_b;

    // Шаг интегрирования по времени (dt).
    double time_step_dt;

    // Текущее значение выхода. Инициализируется прямо здесь,
    // чтобы SonarQube не ругался на конструктор.
    double y_harmonic = 0.0;

public:
    // Конструктор: сохраняет коэффициенты.
    explicit HarmonicODE(double b_, double dt_ = 0.01);

    // Один шаг метода Эйлера для гармонической модели.
    double advance(double u) override;

    // Сброс состояния к начальному (y = 0).
    void reset() override;

    // Название модели для вывода в консоль.
    std::string title() const override;
};
