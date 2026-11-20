#pragma once
// Подключаем базовый класс из ПЕРВОЙ лабы 
#include "../../task_01/src/Model.h" 

class DeadZoneModel : public Model {
private:
    double a, b, delta, y;
public:
    // Конструктор
    DeadZoneModel(double a_, double b_, double delta_) : a(a_), b(b_), delta(delta_), y(0.0) {}
    
    // Сброс состояния
    void reset() override { y = 0.0; }
    
    // Шаг симуляции
    double step(double u) override {
        double u_eff = 0.0;
        // Если сигнал больше зоны нечувствительности, он проходит
        if (u > delta) u_eff = u - delta;
        else if (u < -delta) u_eff = u + delta;
        // Если внутри зоны (от -delta до delta), u_eff остается 0
        
        y = a * y + b * u_eff;
        return y;
    }
    
    const char* name() override { return "Dead Zone Model"; }
};