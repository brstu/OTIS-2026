#pragma once
#include "DynamicModel.h"
#include <sstream>

// Model 1.3: y_{t+1} = a1*y_t + a2*y_{t-1} + b*u_t
class Model1_3 : public DynamicModel
{
private:
    double a1, a2, b;
    double y, y_prev;

public:
    Model1_3(double a1_, double a2_, double b_)
        : a1(a1_), a2(a2_), b(b_), y(0.0), y_prev(0.0) {
    }

    double advance(double u) override
    {
        double y_next = a1 * y + a2 * y_prev + b * u;
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
        return "Model 1.3 (Oscillatory)";
    }
};