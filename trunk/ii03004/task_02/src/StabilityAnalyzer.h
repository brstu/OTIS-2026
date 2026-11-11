#pragma once

class StabilityAnalyzer {
public:
    // Проверка устойчивости линейной модели по коэффициенту a1 (корень характеристического уравнения z = a1)
    static bool isLinearSystemStable(double a1);
    
    // Возвращает значение корня на Z-плоскости
    static double calculateRoot(double a1);
};