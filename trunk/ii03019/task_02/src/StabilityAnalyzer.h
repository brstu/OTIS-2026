#pragma once

class StabilityAnalyzer {
public:
    // Проверка корня характеристического уравнения z - a = 0
    // Система устойчива, если |a| < 1
    static bool isStable(double a);
};