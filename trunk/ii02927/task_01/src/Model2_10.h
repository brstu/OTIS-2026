#pragma once
#include "DynamicModel.h"
#include <cmath>
#include <sstream>

// Model 2.10: y_{t+1} = a*tanh(y_t) + b*u_t^3
class Model2_10 : public DynamicModel
{
private:
    double a;
    double b;
    double y = 0.0;

public:
    Model2_10(double a_, double b_) : a(a_), b(b_) {}

    double advance(double u) override
    {
        y = a * std::tanh(y) + b * (u * u * u);
        return y;
    }

    void reset() override
    {
        y = 0.0;
    }

    std::string describe() const override
    {
        return "Model 2.10 (Hyperbolic Tangent)";
    }
};