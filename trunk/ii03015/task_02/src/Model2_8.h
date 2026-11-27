#pragma once
#include "Model.h"

class Model2_8 final : public Model
{
private:
    double a;
    double b;
    double c;
public:
    Model2_8(double a, double b, double c);

    double computeNext(double y, double yPrev, double yPrev2,
                         double u, double dt) const override;

    const char* label() const override;

    double getA() const;
    double getB() const;
    double getC() const;
};
