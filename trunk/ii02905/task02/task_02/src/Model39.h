
#pragma once

#include "Model.h"

class Model39 : public Model
{
private:
    double y;
    double b;
    double dt;

public:
    Model39(double y0, double b, double dt);

    double step(double u, int tau) override;

    double getY() const override;
};
