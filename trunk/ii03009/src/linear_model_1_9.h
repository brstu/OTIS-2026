#pragma once
#include "model.h"

class LinearModel1_9 : public Model {
public:
    LinearModel1_9(double a1, double a2, double a3, double b1);
    double step(double u) override;
    void reset() override;
    std::string getName() const override { return "Linear 1.9"; }

private:
    double a1_, a2_, a3_, b1_;
    double y_prev1_, y_prev2_; 
    double y_;
};