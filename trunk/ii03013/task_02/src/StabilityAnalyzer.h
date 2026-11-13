#pragma once

#include <complex>
#include <string>
#include <vector>

// Анализ устойчивости линейных дискретных систем.
// Система устойчива, если все корни характеристического уравнения
// лежат строго внутри единичного круга Z-плоскости: |z| < 1.
namespace stability {

struct Report {
    bool stable = true;
    std::vector<std::complex<double>> roots;
    double maxModulus = 0.0;   // спектральный радиус max|z_i|
};

// Корни многочлена c[0]*z^n + c[1]*z^(n-1) + ... + c[n] = 0
// (коэффициенты от старшей степени к младшей).
// Бросает std::invalid_argument для пустого/константного многочлена,
// нулевого старшего коэффициента и нечисловых (NaN/inf) коэффициентов.
std::vector<std::complex<double>> findRoots(const std::vector<double>& coeffs);

// Спектральный радиус: максимальный модуль среди корней (0 для пустого набора).
double spectralRadius(const std::vector<std::complex<double>>& roots);

// Полный анализ: корни, максимальный модуль, вердикт.
// Граница |z| = 1 считается НЕустойчивой 
Report analyze(const std::vector<double>& coeffs, double tolerance = 1e-9);

bool isStable(const std::vector<double>& coeffs, double tolerance = 1e-9);

// Текст предупреждения для консоли. Для устойчивой системы возвращает "".
std::string makeWarning(const std::string& modelName, const Report& report);

} // namespace stability
