#pragma once

class StabilityAnalyzer {
public:
    // Проверка для дискретных моделей (1.10 и 2.5)
    static bool isDiscreteStable(double a);
    
    // Проверка для модели с дифференциальным уравнением (3.10)
    static bool isEulerStable(double a, double dt);
};