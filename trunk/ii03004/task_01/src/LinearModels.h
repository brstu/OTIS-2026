#pragma once

#include "Model.h"

// Model 1.4 (Discrete Integrator with Loss):
// y(tau+1) = a*y(tau) + b*(u(tau) - u(tau-1))
class Model1_4 final : public Model
{
public:
    Model1_4(double a, double b);

    std::string name() const override;
    std::string formula() const override;

    // Характеристическое уравнение: z - a = 0  =>  z = a.
    // Процесс устойчив, если корень лежит внутри единичного круга: |a| < 1.
    bool isStable() const;

protected:
    double calculateNext(double u) const override;

private:
    double a_;
    double b_;
};
