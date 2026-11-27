#include "Model1_6.h"

Model1_6::Model1_6(double a1, double a2, double a3, double b)
    : a1(a1), a2(a2), a3(a3), b(b) {}

double Model1_6::computeNext(double y, double yPrev, double yPrev2,
                               double u, double ) const
{

    return a1 * y + a2 * yPrev + a3 * yPrev2 + b * u;
}

const char* Model1_6::label() const
{
    return "Model 1.6 - Third-Order Dynamic System";
}

double Model1_6::getA1() const { return a1; }
double Model1_6::getA2() const { return a2; }
double Model1_6::getA3() const { return a3; }
double Model1_6::getB()  const { return b;  }
