
#pragma once

#include "Model.h"

class Model27 : public Model
{
private:
    double y;
    double a;
    double b;
    double epsilon;
    int relayState = 0;

    double relay(double u);

public:
    Model27(double y0, double a, double b, double epsilon);

    double step(double u, int tau) override;

    double getY() const override;
};
