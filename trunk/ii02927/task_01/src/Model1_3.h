#pragma once
#include "DynamicModel.h"
#include <sstream>

class Model1_3 : public DynamicModel
{
private:
    double a1;
    double a2;
    double b;
    double y = 0.0;
    double y_prev = 0.0;

public:
    Model1_3(double a1_, double a2_, double b_)
        : a1(a1_), a2(a2_), b(b_) 
    {
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
