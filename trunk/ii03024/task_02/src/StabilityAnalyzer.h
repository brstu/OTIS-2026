#ifndef STABILITY_ANALYZER_H
#define STABILITY_ANALYZER_H

#include <iosfwd>
#include <string>

// Результат анализа устойчивости дискретной системы.
enum class Stability {
    Stable,    // все корни внутри единичного круга (|z| < 1)
    Marginal,  // корень на границе (|z| = 1): процесс не затухает
    Unstable   // корень вне единичного круга (|z| > 1) или коэффициент не число
};

// Анализ устойчивости линейной модели 1.1:  y(t+1) = a*y(t) + b*u(t).
//
// Z-преобразование:  z*Y(z) = a*Y(z) + b*U(z)
// Характеристическое уравнение:  z - a = 0   =>   z = a.
// Система асимптотически устойчива  <=>  |a| < 1.
class StabilityAnalyzer {
public:
    // Допуск, в пределах которого |z| считается равным 1.
    static constexpr double kEps = 1e-9;

    // Единственный корень характеристического уравнения z - a = 0.
    static double characteristicRoot(double a);

    // Классификация по расположению корня относительно единичной окружности.
    static Stability classify(double a, double eps = kEps);

    // true только для строго устойчивой системы (|a| < 1).
    static bool isStable(double a, double eps = kEps);

    // Текст предупреждения; пустая строка, если система устойчива.
    static std::string warningMessage(double a, double eps = kEps);

    // Печатает предупреждение в поток (если нужно). Возвращает true, если система устойчива.
    static bool warnIfUnstable(double a, std::ostream& out, double eps = kEps);

    // Бросает std::domain_error, если система не является устойчивой.
    static void requireStable(double a, double eps = kEps);
};

#endif
