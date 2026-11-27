#pragma once
#include "DynamicModel.h"
#include <string>

class Model1_6 : public DynamicModel
{
private:
    double a1;
    double a2;
    double a3;
    double b;
    double y = 0.0;
    double y_prev1 = 0.0;
    double y_prev2 = 0.0;

public:
    Model1_6(double a1_, double a2_, double a3_, double b_)
        : a1(a1_), a2(a2_), a3(a3_), b(b_)
    {
    }

    double advance(double u) override
    {
        double y_next = a1 * y + a2 * y_prev1 + a3 * y_prev2 + b * u;
        y_prev2 = y_prev1;
        y_prev1 = y;
        y = y_next;
        return y;
    }

    void reset() override
    {
        y = 0.0;
        y_prev1 = 0.0;
        y_prev2 = 0.0;
    }

    std::string describe() const override
    {
        return "Model 1.6 (Third-Order System)";
    }
};