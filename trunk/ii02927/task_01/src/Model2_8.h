#pragma once
#include "DynamicModel.h"
#include <cmath>
#include <string>

// Model 2.8: y_{t+1} = a*y_t*(1-y_t) + b*u_t + c*sin(y_{t-1}*u_t)
class Model2_8 : public DynamicModel
{
private:
    double a;
    double b;
    double c;
    double y = 0.0;
    double y_prev = 0.0;

public:
    Model2_8(double a_, double b_, double c_) : a(a_), b(b_), c(c_) {}

    double advance(double u) override
    {
        double y_next = a * y * (1.0 - y) + b * u + c * std::sin(y_prev * u);
        y_prev = y;
        y = y_next;
        return y;
    }

    void reset() override
    {
        y = 0.0;
        y_prev = 0.0;
    }

    std::string describe() const override
    {
        return "Model 2.8 (Chaotic Logistic Map)";
    }
};