#pragma once

#include "Model.h"

// Model 2.6 (Asymmetric Polynomial Backlash Simulation):
// y(tau+1) = a1*y(tau)^3 - a2*y(tau-1) + b*u(tau)^2
class Model2_6 final : public Model
{
public:
    Model2_6(double a1, double a2, double b);

    std::string name() const override;
    std::string formula() const override;

protected:
    double calculateNext(double u) const override;

private:
    double a1_;
    double a2_;
    double b_;
};
