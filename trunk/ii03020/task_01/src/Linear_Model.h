#pragma once
#include "Model.h"
#include "InputGenerator.h"

class Linear_Model : public Model {
    double a, b1, b2, b3;
    double y, u_t, u_t1, u_t2, last_u;
    InputGenerator input;
public:
    Linear_Model(double a, double b1, double b2, double b3,
                 int type, double u_const);
    double NextStep() override;
    double GetY()     const override;
    double GetLastU() const override;
};