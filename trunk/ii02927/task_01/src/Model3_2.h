#pragma once
#include "DynamicModel.h"
#include <sstream>

// Model 3.2: dy/dt = b*u (Метод Эйлера)
class Model3_2 : public DynamicModel
{
private:
    double b;
    double dt;
    double y = 0.0;

public:
    Model3_2(double b_, double dt_) : b(b_), dt(dt_) {}

    double advance(double u) override
    {
        y = y + dt * (b * u);
        return y;
    }

    void reset() override
    {
        y = 0.0;
    }

    std::string describe() const override
    {
        return "Model 3.2 (Pure Input Drive)";
    }
};