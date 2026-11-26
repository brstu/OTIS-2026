#ifndef MODELS_H
#define MODELS_H

#include "Model.h"
#include <complex>
#include <string>
#include <vector>

// 1. Линейная модель (1-го порядка): y(k) = a * y(k-1) + b * u(k)
// Характеристическое уравнение: z - a = 0 => z = a
// Устойчивость: |a| < 1
class LinearModel1_1 : public Model {
private:
    double a;
    double b;

public:
    LinearModel1_1(double coefficient_a, double coefficient_b);
    double calculateNextStep(double u, double y_prev) override;
    bool isStable() const override;
    void printStabilityInfo() const override;
    double getA() const { return a; }
    double getB() const { return b; }
};

// 2. Нелинейная модель с зоной нечувствительности (Model 2.3)
// y(k) = a * y(k-1) + b * deadzone(u(k), delta)
// Линеаризованная модель: y(k) = a * y(k-1) + b * u(k) (для |u| > delta)
// Характеристическое уравнение: z - a = 0 => z = a
// Устойчивость: |a| < 1
class NonLinearModel2_3 : public Model {
private:
    double a;
    double b;
    double delta;
    double deadzone(double u);

public:
    NonLinearModel2_3(double coefficient_a, double coefficient_b, double d);
    double calculateNextStep(double u, double y_prev) override;
    bool isStable() const override;
    void printStabilityInfo() const override;
    double getA() const { return a; }
    double getB() const { return b; }
    double getDelta() const { return delta; }
};

// 3. Модель на основе дифференциального уравнения (2-го порядка)
// y(k) = a1 * y(k-1) + a2 * y(k-2) + b0 * u(k)
// Характеристическое уравнение: z^2 - a1*z - a2 = 0
// Устойчивость: все корни внутри единичного круга |z| < 1
// Критерий Юра:
//   1) |a2| < 1
//   2) |a1| < 1 + a2
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
    bool isStable() const override;
    void printStabilityInfo() const override;
    double getA1() const { return a1; }
    double getA2() const { return a2; }
    double getB0() const { return b0; }

    // Получить корни характеристического уравнения
    std::vector<std::complex<double>> getCharacteristicRoots() const;
};

// Вспомогательные функции симуляции и проверки устойчивости (перенесены из Utils)
void saveCSV(const std::string& filename, const std::vector<double>& steps, const std::vector<double>& outputs);
void runSimulation(Model& model, const std::string& prefix, int n, int inputType, double customU);
bool checkStabilityAndWarn(Model& model);

#endif