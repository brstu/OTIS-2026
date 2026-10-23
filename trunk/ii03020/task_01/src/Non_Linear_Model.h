#pragma once
#include "Model.h"
#include "InputGenerator.h"

class Non_linear_Model : public Model {
    double a, b, c, d;
    double y, y_prev, u_t, u_t1, last_u;
    InputGenerator input;
public:
    Non_linear_Model(double a, double b, double c, double d,
                     int type, double u_const);
    double NextStep() override;
    double GetY()     const override;
    double GetLastU() const override;
};