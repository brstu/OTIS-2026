#include "Models.h"
#include <cmath>

// --- Линейная модель ---
LinearModel1_1::LinearModel1_1(double coefficient_a, double coefficient_b)
    : a(coefficient_a), b(coefficient_b) {}

double LinearModel1_1::calculateNextStep(double u, double y_prev) {
    return a * y_prev + b * u;
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