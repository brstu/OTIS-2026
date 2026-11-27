#include "Model2_8.h"
#include <cmath>

Model2_8::Model2_8(double a, double b, double c)
    : a(a), b(b), c(c) {}

double Model2_8::computeNext(double y, double yPrev, double ,
                               double u, double ) const
{

    return a * y * (1.0 - y) + b * u + c * std::sin(yPrev * u);
}

const char* Model2_8::label() const
{
    return "Model 2.8 - Chaotic Logistic Map Disturbance";
}

double Model2_8::getA() const { return a; }
double Model2_8::getB() const { return b; }
double Model2_8::getC() const { return c; }
