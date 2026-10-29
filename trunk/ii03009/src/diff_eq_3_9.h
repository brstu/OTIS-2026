#pragma once
#include "model.h"

class DiffEq3_9 : public Model {
public:
    DiffEq3_9(double b, double dt); // dt - шаг времени
    double step(double u) override;
    void reset() override;
    std::string getName() const override { return "DiffEq 3.9"; }

private:
    double b_;
    double dt_;
    double y_;
};