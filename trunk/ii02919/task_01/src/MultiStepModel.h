#pragma once

#include "DynamicProcess.h"

// =====================================================================
// MultiStepModel — Model 1.7 (System with Multi-Step Control History)
//
// Формула:
//     y(t+1) = a*y(t) + b1*u(t) + b2*u(t-1) + b3*u(t-2)
//
// Модель хранит ДВА предыдущих значения входа (u_prev1, u_prev2),
// чтобы учитывать задержку управления.
// =====================================================================
class MultiStepModel : public DynamicProcess
{
private:
    // Коэффициенты уравнения.
    double coef_a;
    double coef_b1;
    double coef_b2;
    double coef_b3;

    // Состояние модели (уникальные имена переменных).
    double y_step = 0.0;         // текущее значение выхода
    double u_history_1 = 0.0;    // u(t-1)
    double u_history_2 = 0.0;    // u(t-2)

public:
    // Конструктор.
    MultiStepModel(double a_, double b1_, double b2_, double b3_);

    // Один шаг симуляции.
    double advance(double u) override;

    // Сброс состояния.
    void reset() override;

    // Название модели.
    std::string title() const override;

    // Проверка устойчивости (по корню z = a).
    bool isStableSystem() const override;

    // Текст предупреждения о неустойчивости.
    std::string alertText() const override;
};
