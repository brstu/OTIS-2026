#pragma once

#include "Model.h"

// y[tau+1] = a*y[tau] + b*sign(u[tau])*(1 - e^(-|u[tau]|))
class NonlinearModel_2_5 : public Model {
private:
    double a;
    double b;
    double y;

public:
    NonlinearModel_2_5(double a_, double b_);
    void reset() override;
    double step(double u) override;
    const char* name() override;
};