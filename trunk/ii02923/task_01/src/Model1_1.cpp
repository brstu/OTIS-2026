#include "Model1_1.h"

#include "StabilityAnalyzer.h"

Model1_1::Model1_1(double a, double b) : a_(a), b_(b) {}

void Model1_1::reset() {
    y_ = 0.0;
}

double Model1_1::step(double u, double) {
    y_ = a_ * y_ + b_ * u;
    return y_;
}

std::string Model1_1::name() const {
    return "Model 1.1";
}

std::string Model1_1::equation() const {
    return "y[t+1] = a*y[t] + b*u[t]";
}

bool Model1_1::isStable() const {
    return StabilityAnalyzer::isStableFirstOrder(a_);
}

double Model1_1::characteristicRoot() const {
    return a_;
}
