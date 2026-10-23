#pragma once
#include "Model.h"
#include "InputGenerator.h"

class Simple_Differential_Equation : public Model {
    double b, dt;
    double y, last_u;
    InputGenerator input;
public:
    Simple_Differential_Equation(double b, double dt,
                                 int type, double u_const);
    double NextStep() override;
    double GetY()     const override;
    double GetLastU() const override;
};