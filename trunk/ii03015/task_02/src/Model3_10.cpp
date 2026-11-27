#include "Model3_10.h"

Model3_10::Model3_10(double a, double b)
    : a(a), b(b) {}

double Model3_10::computeNext(double y, double , double ,
                                double u, double dt) const
{

    return y + dt * (-a * y + b + u);
}

const char* Model3_10::label() const
{
    return "Model 3.10 - Basic External Constant Offset";
}

double Model3_10::getA() const { return a; }
double Model3_10::getB() const { return b; }
