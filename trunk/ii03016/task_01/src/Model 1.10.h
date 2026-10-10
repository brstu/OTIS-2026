#pragma once

#include "Model.h"

// y[t+1] = a1*y[t] + b1*u[t] + b2*u[t-2]
class LineModel_1_10 : public Model
{
private:
    double a1;
    double b1;
    double b2;
    double y;
    double u_prev1;
    double u_prev2;

public:
    LineModel_1_10(double a1_, double b1_, double b2_);
    void reset() override;
    double step(double u) override;
    const char *name() override;
};