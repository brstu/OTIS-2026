#include "StabilityAnalyzer.h"
#include <cmath> // Нужно для функции std::fabs (модуль числа)

// Для моделей 1.10 и 2.5: корень z = a, устойчиво если |a| < 1
bool StabilityAnalyzer::isDiscreteStable(double a) {
    return std::fabs(a) < 1.0;
}

// Для модели 3.10: корень z = 1 - a*dt, устойчиво если |1 - a*dt| < 1
bool StabilityAnalyzer::isEulerStable(double a, double dt) {
    return std::fabs(1.0 - a * dt) < 1.0;
}