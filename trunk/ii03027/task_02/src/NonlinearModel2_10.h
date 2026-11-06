#pragma once
#include "Model.h"

class NonlinearModel2_10 : public Model {
private:
    double a, b, y_t;
public:
    NonlinearModel2_10(double _a, double _b);
    void reset() override;
    double step(double u) override;
};