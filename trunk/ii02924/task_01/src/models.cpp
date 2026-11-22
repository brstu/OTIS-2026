#include "models.hpp"

#include <cmath>

LinearModel19::LinearModel19(double a1, double a2, double a3, double b1)
    : a1_(a1), a2_(a2), a3_(a3), b1_(b1) {}

double LinearModel19::next(double u, double) {
    const double result = a1_ * y0_ + a2_ * y1_ + a3_ * y2_ + b1_ * u;
    y2_ = y1_;
    y1_ = y0_;
    y0_ = result;
    return result;
}

void LinearModel19::reset() {
    y0_ = 0.0;
    y1_ = 0.0;
    y2_ = 0.0;
}

std::string LinearModel19::name() const {
    return "linear_1_9";
}

NonlinearModel21::NonlinearModel21(double a, double b, double c, double d)
    : a_(a), b_(b), c_(c), d_(d) {}

double NonlinearModel21::next(double u, double previousU) {
    const double result = a_ * y0_ - b_ * y1_ * y1_ + c_ * u
                        + d_ * std::sin(previousU);
    y1_ = y0_;
    y0_ = result;
    return result;
}

void NonlinearModel21::reset() {
    y0_ = 0.0;
    y1_ = 0.0;
}

std::string NonlinearModel21::name() const {
    return "nonlinear_2_1";
}

DifferentialModel33::DifferentialModel33(double a, double b, double dt)
    : a_(a), b_(b), dt_(dt) {}

double DifferentialModel33::next(double u, double) {
    y_ += dt_ * (-a_ * y_ + b_ * u);
    return y_;
}

void DifferentialModel33::reset() {
    y_ = 0.0;
}

std::string DifferentialModel33::name() const {
    return "differential_3_3";
}
