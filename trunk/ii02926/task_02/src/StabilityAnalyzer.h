#pragma once

#include <complex>
#include <string>

// Модуль анализа устойчивости на Z-плоскости.
//
// Для линейной дискретной системы устойчивость определяется корнями
// характеристического уравнения. Здесь используются два случая варианта 7:
//   * модели 1.7 и 2.9:  y[t+1] = a*y[t] + (...)   =>  z - a = 0,       z = a
//   * схема Эйлера 3.1:  y[t+1] = (1 - a*dt)*y[t]  =>  z - (1-a*dt) = 0, z = 1 - a*dt
// Асимптотическая устойчивость <=> |z| < 1. Граница |z| = 1 устойчивой НЕ считается.
namespace stability {

enum class Class {
    Stable,    // |z| < 1
    Boundary,  // |z| = 1 (с допуском kBoundaryEps)
    Unstable,  // |z| > 1
    Invalid    // параметры не конечны или dt <= 0 - анализ невозможен
};

constexpr double kBoundaryEps = 1e-12;

// Корни характеристических уравнений (комплексный тип - для единообразия Z-плоскости)
std::complex<double> firstOrderRoot(double a);
std::complex<double> eulerRoot(double a, double dt);

// Классификация по модулю корня
Class classifyRoot(std::complex<double> z);
Class classifyFirstOrder(double a);
Class classifyEuler(double a, double dt);

inline bool isStable(Class c) { return c == Class::Stable; }

// Текст предупреждения. Для устойчивой системы возвращает пустую строку.
std::string firstOrderWarning(const std::string& tag, double a);
std::string eulerWarning(const std::string& tag, double a, double dt);

}  // namespace stability
