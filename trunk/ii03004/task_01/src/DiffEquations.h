#pragma once

#include "Model.h"

// Model 3.8 (Combined Linear-Quadratic Decay):
// dy/dt = -a1*y - a2*y^2 + b*u
//
// Решается методом Эйлера с шагом dt:
// y(tau+1) = y(tau) + dt*(-a1*y(tau) - a2*y(tau)^2 + b*u(tau))
class Model3_8 final : public Model
{
public:
    Model3_8(double a1, double a2, double b, double dt);

    std::string name() const override;
    std::string formula() const override;

protected:
    double calculateNext(double u) const override;

private:
    double a1_;
    double a2_;
    double b_;
    double dt_;
};
