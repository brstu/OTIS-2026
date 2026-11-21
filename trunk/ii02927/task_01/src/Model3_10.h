#pragma once
#include "DynamicModel.h"
#include <string>

// Model 3.10: dy/dt = -a*y + b + u
class Model3_10 : public DynamicModel
{
private:
    double a;
    double b;
    double dt;
    double y = 0.0;

public:
    Model3_10(double a_, double b_, double dt_) : a(a_), b(b_), dt(dt_) {}

    double advance(double u) override
    {
        // Метод Эйлера
        y = y + dt * (-a * y + b + u);
        return y;
    }

    void reset() override
    {
        y = 0.0;
    }

    std::string describe() const override
    {
        return "Model 3.10 (External Constant Offset)";
    }
};