#pragma once
#include "Model.h"

class Model1_6 final : public Model
{
private:
    double a1;
    double a2;
    double a3;
    double b;

public:
    Model1_6(double a1, double a2, double a3, double b);

    double computeNext(double y, double yPrev, double yPrev2,
                         double u, double dt) const override;

    const char* label() const override;

    double getA1() const;
    double getA2() const;
    double getA3() const;
    double getB() const;
};
