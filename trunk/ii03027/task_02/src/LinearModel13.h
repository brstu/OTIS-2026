#pragma once
#include "Model.h"

class LinearModel13 : public Model {
private:
    double a1, a2, b, y_t, y_t_minus_1;
public:
    LinearModel13(double _a1, double _a2, double _b);
    void reset() override;
    double step(double u) override;
    double getA1() const { return a1; }
    double getA2() const { return a2; }
};