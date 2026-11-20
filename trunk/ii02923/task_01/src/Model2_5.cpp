#include "Model2_5.h"

#include <cmath>

#include "StabilityAnalyzer.h"

Model2_5::Model2_5(double a, double b) : a_(a), b_(b) {}

void Model2_5::reset() {
    y_ = 0.0;
}

double Model2_5::step(double u, double) {
    // sign(u) * (1 - exp(-|u|)) — гладкий аналог сухого трения:
    // при u = 0 даёт 0, при |u| -> бесконечность стремится к sign(u).
    const double friction = (u == 0.0)
                                ? 0.0
                                : std::copysign(1.0, u) * (1.0 - std::exp(-std::fabs(u)));
    y_ = a_ * y_ + b_ * friction;
    return y_;
}

std::string Model2_5::name() const {
    return "Model 2.5";
}

std::string Model2_5::equation() const {
    return "y[t+1] = a*y[t] + b*sign(u[t])*(1 - exp(-|u[t]|))";
}

bool Model2_5::isStable() const {
    // Локальная устойчивость по линеаризации в точке u = 0:
    // sign(u)*(1-exp(-|u|)) ~ u, поэтому корень характеристического
    // уравнения снова равен a.
    return StabilityAnalyzer::isStableFirstOrder(a_);
}

double Model2_5::characteristicRoot() const {
    return a_;
}
