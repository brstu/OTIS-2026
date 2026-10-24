#pragma once

#include "DynamicProcess.h"

// ============================================================================
//  MultiStepModel — Model 1.7
//  System with Multi-Step Control History
//
//  Формула:
//    y(t+1) = a*y(t) + b1*u(t) + b2*u(t-1) + b3*u(t-2)
//
//  Система хранит два предыдущих значения входа (u_{t-1}, u_{t-2}),
//  чтобы учитывать задержку управления.
// ============================================================================
class MultiStepModel : public DynamicProcess
{
private:
    double a;      // коэффициент затухания
    double b1;     // коэффициент текущего входа
    double b2;     // коэффициент входа с задержкой 1
    double b3;     // коэффициент входа с задержкой 2

    double y;         // текущее значение выхода
    double u_prev1;   // u(t-1)
    double u_prev2;   // u(t-2)

public:
    MultiStepModel(double a_, double b1_, double b2_, double b3_);

    double advance(double u) override;
    void reset() override;
    std::string title() const override;

    bool isStableSystem() const override;
    std::string alertText() const override;
};
