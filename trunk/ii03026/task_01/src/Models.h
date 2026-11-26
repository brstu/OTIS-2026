#ifndef MODELS_H
#define MODELS_H

#include "Model.h"

// 1. Линейная модель
class LinearModel1_1 : public Model {
private:
    double a;
    double b;

public:
    LinearModel1_1(double coefficient_a, double coefficient_b);
    double calculateNextStep(double u, double y_prev) override;
};

// 2. Нелинейная модель с зоной нечувствительности
class NonLinearModel2_3 : public Model {
private:
    double a;
    double b;
    double delta;
    double deadzone(double u);

public:
    NonLinearModel2_3(double coefficient_a, double coefficient_b, double d);
    double calculateNextStep(double u, double y_prev) override;
};

// 3. Модель на основе дифференциального уравнения (разностное уравнение 2-го порядка)
class DifferentialModel : public Model {
private:
    double a1; // коэффициент при y(k-1)
    double a2; // коэффициент при y(k-2)
    double b0; // коэффициент при u(k)

    // Внутренняя память для хранения предыстории выходов (нужна для 2-го порядка)
    double y_prev1 = 0.0;
    double y_prev2 = 0.0;
    bool firstStep = true;

public:
    DifferentialModel(double coef_a1, double coef_a2, double coef_b0);
    double calculateNextStep(double u, double y_prev) override;
};

#endif