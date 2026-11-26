#include "Models.h"
#include <cmath>
#include <complex>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <vector>

// --- Линейная модель ---
LinearModel1_1::LinearModel1_1(double coefficient_a, double coefficient_b)
    : a(coefficient_a), b(coefficient_b) {}

double LinearModel1_1::calculateNextStep(double u, double y_prev) {
    return a * y_prev + b * u;
}

bool LinearModel1_1::isStable() const {
    // Характеристическое уравнение: z - a = 0 => z = a
    // Устойчивость: |a| < 1 (корень внутри единичного круга)
    return std::abs(a) < 1.0;
}

void LinearModel1_1::printStabilityInfo() const {
    std::cout << std::fixed << std::setprecision(4);
    std::cout << "=== Stability Analysis: Linear Model (1st Order) ===\n";
    std::cout << "Characteristic equation: z - a = 0\n";
    std::cout << "Root: z = " << a << "\n";
    std::cout << "Root magnitude: |z| = " << std::abs(a) << "\n";
    std::cout << "Stability criterion: |a| < 1\n";
    if (isStable()) {
        std::cout << "Result: STABLE (|a| = " << std::abs(a) << " < 1)\n";
    } else {
        std::cout << "Result: UNSTABLE (|a| = " << std::abs(a) << " >= 1) - WARNING!\n";
    }
    std::cout << "======================================================\n";
}

// --- Нелинейная модель (с зоной нечувствительности) ---
NonLinearModel2_3::NonLinearModel2_3(double coefficient_a, double coefficient_b, double d)
    : a(coefficient_a), b(coefficient_b), delta(d) {}

double NonLinearModel2_3::deadzone(double u) {
    return (std::abs(u) > delta) ? (u > 0 ? u - delta : u + delta) : 0.0;
}

double NonLinearModel2_3::calculateNextStep(double u, double y_prev) {
    double processed_u = deadzone(u);
    return a * y_prev + b * processed_u;
}

bool NonLinearModel2_3::isStable() const {
    // Для линеаризованной модели: y(k) = a * y(k-1) + b * u(k)
    // Характеристическое уравнение: z - a = 0 => z = a
    // Устойчивость: |a| < 1
    return std::abs(a) < 1.0;
}

void NonLinearModel2_3::printStabilityInfo() const {
    std::cout << std::fixed << std::setprecision(4);
    std::cout << "=== Stability Analysis: Nonlinear Model (Deadzone) ===\n";
    std::cout << "Linearized model: y(k) = a * y(k-1) + b * u(k)\n";
    std::cout << "Characteristic equation: z - a = 0\n";
    std::cout << "Root: z = " << a << "\n";
    std::cout << "Root magnitude: |z| = " << std::abs(a) << "\n";
    std::cout << "Deadzone parameter: delta = " << delta << "\n";
    std::cout << "Stability criterion: |a| < 1\n";
    if (isStable()) {
        std::cout << "Result: STABLE (|a| = " << std::abs(a) << " < 1)\n";
    } else {
        std::cout << "Result: UNSTABLE (|a| = " << std::abs(a) << " >= 1) - WARNING!\n";
    }
    std::cout << "====================================================\n";
}

// --- Модель дифференциального уравнения (2-го порядка) ---
DifferentialModel::DifferentialModel(double coef_a1, double coef_a2, double coef_b0)
    : a1(coef_a1), a2(coef_a2), b0(coef_b0) {}

double DifferentialModel::calculateNextStep(double u, double y_prev_ignored) {
    double y_current;

    if (firstStep) {
        y_current = b0 * u;
        y_prev2 = 0.0;
        y_prev1 = y_current;
        firstStep = false;
    } else {
        // Разностное уравнение второго порядка
        y_current = a1 * y_prev1 + a2 * y_prev2 + b0 * u;

        // Сдвиг предыстории для следующих шагов
        y_prev2 = y_prev1;
        y_prev1 = y_current;
    }

    return y_current;
}

std::vector<std::complex<double>> DifferentialModel::getCharacteristicRoots() const {
    // Характеристическое уравнение: z^2 - a1*z - a2 = 0
    // Корни: z = (a1 +/- sqrt(a1^2 + 4*a2)) / 2
    std::vector<std::complex<double>> roots;
    double discriminant = a1 * a1 + 4.0 * a2;

    if (discriminant >= 0) {
        double sqrt_disc = std::sqrt(discriminant);
        roots.push_back(std::complex<double>((a1 + sqrt_disc) / 2.0, 0.0));
        roots.push_back(std::complex<double>((a1 - sqrt_disc) / 2.0, 0.0));
    } else {
        double real_part = a1 / 2.0;
        double imag_part = std::sqrt(-discriminant) / 2.0;
        roots.push_back(std::complex<double>(real_part, imag_part));
        roots.push_back(std::complex<double>(real_part, -imag_part));
    }
    return roots;
}

bool DifferentialModel::isStable() const {
    // Критерий Юра для полинома z^2 - a1*z - a2 = 0:
    // 1) |a2| < 1
    // 2) |a1| < 1 + a2
    auto roots = getCharacteristicRoots();
    for (const auto& root : roots) {
        if (std::abs(root) >= 1.0) {
            return false;
        }
    }
    return true;
}

void DifferentialModel::printStabilityInfo() const {
    std::cout << std::fixed << std::setprecision(4);
    std::cout << "=== Stability Analysis: Differential Model (2nd Order) ===\n";
    std::cout << "Difference equation: y(k) = a1*y(k-1) + a2*y(k-2) + b0*u(k)\n";
    std::cout << "Characteristic equation: z^2 - a1*z - a2 = 0\n";

    auto roots = getCharacteristicRoots();
    std::cout << "Roots:\n";
    for (size_t i = 0; i < roots.size(); ++i) {
        std::cout << "  z" << (i + 1) << " = " << roots[i].real();
        if (roots[i].imag() >= 0) std::cout << " + ";
        else std::cout << " ";
        std::cout << roots[i].imag() << "i";
        std::cout << "  |z" << (i + 1) << "| = " << std::abs(roots[i]) << "\n";
    }

    std::cout << "Coefficients: a1 = " << a1 << ", a2 = " << a2 << "\n";
    std::cout << "Jury criterion:\n";
    std::cout << "  1) |a2| < 1:  |" << a2 << "| = " << std::abs(a2) << " " << (std::abs(a2) < 1.0 ? "OK" : "FAIL") << "\n";
    std::cout << "  2) |a1| < 1 + a2:  |" << a1 << "| = " << std::abs(a1) << " < " << (1 + a2) << " " << (std::abs(a1) < 1 + a2 ? "OK" : "FAIL") << "\n";

    if (isStable()) {
        std::cout << "Result: STABLE (all roots inside unit circle)\n";
    } else {
        std::cout << "Result: UNSTABLE (at least one root outside unit circle) - WARNING!\n";
    }
    std::cout << "========================================================\n";
}

// --- Функции сохранения CSV, симуляции и проверки устойчивости ---
void saveCSV(const std::string& filename, const std::vector<double>& steps, const std::vector<double>& outputs) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        // Ошибка открытия файла для записи
        std::cerr << "Error: failed to open file for writing: " << filename << std::endl;
        return;
    }

    file << "Step,Y\n";
    for (size_t i = 0; i < steps.size(); ++i) {
        file << steps[i] << "," << outputs[i] << "\n";
    }

    file.close();
}

void runSimulation(Model& model, const std::string& prefix, int n, int inputType, double customU) {
    std::cout << std::fixed << std::setprecision(4);

    std::vector<double> steps(n);
    std::vector<double> outputs(n);
    double y = 0.0;
    std::string filenameSuffix = "";

    std::cout << "\n========================================\n";
    std::cout << " Step (i) \t Input (u) \t Output (Y) \n";
    std::cout << "========================================\n";

    for (int i = 0; i < n; ++i) {
        double u = 0.0;

        switch (inputType) {
            case 1: // Ступенчатое воздействие
                u = (customU != 0.0) ? customU : 1.0;
                filenameSuffix = "_step.csv";
                break;
            case 2: // Импульсное воздействие
                u = (i == 0) ? ((customU != 0.0) ? customU : 1.0) : 0.0;
                filenameSuffix = "_pulse.csv";
                break;
            case 3: // Гармоническое воздействие
                u = ((customU != 0.0) ? customU : 1.0) * std::sin(static_cast<double>(i));
                filenameSuffix = "_harmonic.csv";
                break;
            default:
                u = 1.0;
                filenameSuffix = "_custom.csv";
                break;
        }

        // Вычисление следующего значения выхода модели
        y = model.calculateNextStep(u, y);
        steps[i] = i;
        outputs[i] = y;

        std::cout << "   " << i << " \t\t " << u << " \t\t " << y << "\n";
    }

    std::cout << "========================================\n";

    saveCSV(prefix + filenameSuffix, steps, outputs);
    std::cout << "Data saved to file: " << prefix + filenameSuffix << "\n";
}

bool checkStabilityAndWarn(Model& model) {
    model.printStabilityInfo();
    if (!model.isStable()) {
        // Предупреждение о неустойчивости модели
        std::cout << "\n!!! WARNING: Model is UNSTABLE !!!\n";
        std::cout << "Simulation may result in a diverging process.\n";
        std::cout << "Changing the coefficients is recommended.\n\n";
        return false;
    }
    std::cout << "Model is stable. Simulation is safe.\n\n";
    return true;
}