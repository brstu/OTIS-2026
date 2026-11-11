#ifndef MODELS_H
#define MODELS_H

#include "Model.h"
#include <cmath>

class LinearModel1_1 : public Model {
    double a, b;
public:
    LinearModel1_1(double a, double b);
    double calculateNextStep(double u, double y_prev) override;
};

class NonLinearModel2_3 : public Model {
    double a, b, delta;
    double deadzone(double u);
public:
    NonLinearModel2_3(double a, double b, double delta);
    double calculateNextStep(double u, double y_prev) override;
};

class DiffModel3_5 : public Model {
    double b, dt;
public:
    DiffModel3_5(double b, double dt);
    double calculateNextStep(double u, double y_prev) override;
};

#endif // MODELS_H